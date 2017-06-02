
#include "process.h"
#include <iostream>
#include <cmath>
#include "kiss_fft.h"
#include <math.h>
#include <iomanip>
#include <fstream>


using namespace std;

#define RESULTS_RING_BUFFER_SIZE 1024*sizeof(void*)
#define PI 3.14159265358979
#define FS 48000
#define max_mics_distance 0.2


process::process( std::string fn, int count ) :
  sample_rate( 1 ),
  channels( 1 ),
  outfilename( fn ),
  count( 0 ),
  //levels( 0 )
  sample_count( 0 ),
  sample_limit_number( 0 ),
  limit_sample_number( false ),
  processing_finished( false ),
  rb_results( RESULTS_RING_BUFFER_SIZE )
{
  set_samples_to_process( count ) ;

  pthread_mutex_init( &mutex_levels, NULL ) ;

  pthread_mutex_lock( &mutex_levels ) ;
  //levels = new float[256] ;
  for (int i=0; i< 256; i++)
    levels[i] = 0 ;
  pthread_mutex_unlock( &mutex_levels ) ;

};


process::~process(){
  //delete [] levels ;
};

// ************************************************************************************************************************
// ***



//Erasable..........................................................................
void createSinusoidSignal (int Npoints, float * buffer){
    float rate, result;
    
    for (int i = 0; i<Npoints; i++){
        rate = 10.0 * i;
        result = cos (rate * PI/180);
        buffer [i] = result;
    }
}
//..................................................................................


//Erasable..........................................................................
void addDelayToSignal (int Npts, float * signal, float * delayed_signal, float delay, int fs){
    int length = Npts;
    int delay_samples = (int) delay/fs;
    
    if (delay > 0){
        for (int i = 0; i<delay_samples; i++){
            delayed_signal[i] = 0;
        }
        int a = 0;
        for (int i = delay_samples; i<length; i++){
            delayed_signal[i] = signal[a];
            a++;
        }
    }else if(delay < 0){
        // to do, if needed
    }else return ;
}
//.................................................................................


float abs (float value){
    value = sqrt(value * value);
    return value;
}

int findMaxIdx (float * signal, int signal_length){
    
    int margin = 5;
    int range = max_mics_distance/340*FS + margin;
    
    //std::cout << "Range: "<<range<< std::endl;
    
    int max_idx = 0;
    
    for (int i = 0; i < range; i++){
        
        if (signal[max_idx] < signal[i]){
            max_idx = i;
        }
        
    }
    for (int i = signal_length-range; i < signal_length; i++){
        
        if (signal[max_idx] < signal[i]){
            max_idx = i;
        }
        
    }
    return max_idx;
}

float finMaxVal (float * signal, int signal_length){
    
    int max_idx;
    float max_val;
    
    max_idx = findMaxIdx (&signal[0], signal_length);
    max_val = signal[max_idx];
    
    return max_val;

}


void fft (int Nfft, float * signal, float * result_fft_RE, float * result_fft_IM){
 
    kiss_fft_cfg cfg = kiss_fft_alloc ( Nfft, 0, 0, 0);
    kiss_fft_cpx cx_in [Nfft];
    kiss_fft_cpx cx_out [Nfft];
 
    for (int i = 0; i<Nfft; i++){
        cx_in[i].r = signal[i];
        cx_in[i].i = 0.0;
    }
    kiss_fft (cfg , cx_in, cx_out);
 
    for (int i = 0; i<Nfft; i++){
        result_fft_RE [i] = cx_out[i].r;
        result_fft_RE [i]  *= 1.0 / (float)Nfft ;
     
        result_fft_IM [i] = cx_out[i].i;
        result_fft_IM [i] *= 1.0 / float(Nfft);
    }
    free(cfg);

}

int xcorr (int Nfft, float * spec1_RE, float * spec1_IM, float * spec2_RE, float * spec2_IM, int fs){
    
    kiss_fft_cfg i_cfg = kiss_fft_alloc( Nfft, 1, NULL, NULL);
    kiss_fft_cpx i_cx_in [Nfft];
    kiss_fft_cpx i_cx_out [Nfft];
    
    float xc[Nfft];
    
    for (int i = 0; i<Nfft; i++){
        i_cx_in[i].r = (spec1_RE[i] * spec2_RE[i] + spec1_IM[i] * spec2_IM[i])/(sqrt( spec1_RE[i] * spec1_RE[i] + spec1_IM[i] * spec1_IM[i] ) * sqrt( spec2_RE[i] * spec2_RE[i] + spec2_IM[i] * spec2_IM[i] ));
        i_cx_in[i].i = (spec1_IM[i] * spec2_RE[i] - spec1_RE[i] * spec2_IM[i])/(sqrt( spec1_RE[i] * spec1_RE[i] + spec1_IM[i] * spec1_IM[i] ) * sqrt( spec2_RE[i] * spec2_RE[i] + spec2_IM[i] * spec2_IM[i] ));
    }
    
    kiss_fft (i_cfg , i_cx_in, i_cx_out);
    
    for (int i = 0; i<Nfft; i++){
        //xc[i] = sqrt( i_cx_out[i].r * i_cx_out[i].r + i_cx_out[i].i * i_cx_out[i].i ); //abs
        xc[i] = i_cx_out[i].r; //real
        xc[i]  *= 1.0 / (float)Nfft ;
    }
    free(i_cfg);

    int max_idx = 0;
    
    max_idx = findMaxIdx(&xc[0], Nfft);
    
    if (max_idx > Nfft/2) max_idx -= Nfft;
        
    return max_idx;
    
}

int delayCalculation (int Nfft, int fs, float * signal1, float * signal2){
//  (!) Returns maximum index (not time delay!)

    //Signal 1
    float result_fft_RE[Nfft], result_fft_IM[Nfft];
    fft(Nfft, &signal1[0], &result_fft_RE[0], &result_fft_IM[0]);
    
    //Signal 2
    float result_fft2_RE[Nfft], result_fft2_IM[Nfft];
    fft(Nfft, &signal2[0], &result_fft2_RE[0], &result_fft2_IM[0]);
    
    //Generalized cross-correlation
    int max_idx = 0;
    max_idx = xcorr(Nfft, &result_fft_RE[0], &result_fft_IM[0], &result_fft2_RE[0], &result_fft2_IM[0], fs);
    
    return max_idx;
    
}

// ***
// ************************************************************************************************************************


void process::callback( float* buf, int Nch, int Nsamples ) {

  pthread_mutex_lock( &mutex_levels ) ;
  // decay levels
  for (int c=0; c<channels; c++ ) {
    levels[c] *= 0.1f ;
  }
  // update levels
  float* rptr = buf ;
  for(int i=0; i<Nsamples; i++ ) {
    for (int c=0; c<Nch; c++ ) {
      float sample = *rptr++ ;
      float sample_abs = (sample<0) ? -sample : sample ;
      if ( levels[c] < sample_abs )
	levels[c] = sample_abs ;
    }
  }
  pthread_mutex_unlock( &mutex_levels ) ;


  // ------ Has processing already finished? ------
  if ( processing_finished )
    return ;


  // ------ write to wav file ------
  sf_write_float( infile, buf, Nsamples * Nch ) ;


  // ------ allocate place to put the results ------
  processed_data* results = new processed_data ;


// ************************************************************************************************************************
// ***
    
    std::cout << std::setprecision(15) << std::fixed;
    
    // Store signals
    int number_mics = 8;
    float signals [number_mics][Nsamples];
    int Nfft = Nsamples;
    
    for (int channel = 0; channel < number_mics; channel++){
        //std::cout << "Signal #"<<channel<< std::endl;
        for (int i = 0; i < Nsamples; i++){
            signals [channel][i] = buf [channel + i*Nch];
            //std::cout << signals[channel][i] << std::endl;
        }
    }

    // Delay calculation
    int ref_channel = 0;
    int delays [number_mics]; //indices!
    float med_delay = 0;
    
    for (int i = 0; i<number_mics; i++){
        delays [i] = delayCalculation (Nfft, FS, &signals[ref_channel][0], &signals[i][0]);
        //delays[i] /= FS; // Nao funciona porque?? FS muito grande? passar para double?
        med_delay += delays[i];
    }
    med_delay = med_delay / Nch;
    
    
    // Send spec information
    float result_fft_RE[Nfft], result_fft_IM[Nfft];
    fft(Nfft, &signals[ref_channel][0], &result_fft_RE[0], &result_fft_IM[0]);
    for (int i = 0; i< Nfft/16/8; i++){
        results->spec[i] = sqrt( result_fft_RE[i] * result_fft_RE[i] + result_fft_IM[i] * result_fft_IM[i] ) ;
        results->spec[i] *= 1.0 / (float)Nfft ;
    }
    results->Nspec = Nfft/16/8;
    
    // Change reference
    //std::cout <<"\nDelays normalizados[ " ;
    for (int i = 0; i<number_mics; i++){
        delays [i] = delays[i] - med_delay;
        //std::cout <<"Delays normalizados["<<i<<"]: "<< delays[i]<< std::endl;
        //std::cout << delays[i] << ", " ;
    }
    //std::cout << " ]" << std::endl;
    
    // Optimization problem
    float pinv[2][8] = {2.500000000000000, 1.767766952966369, 0.000000000000000, -1.767766952966368, -2.500000000000000, -1.767766952966369, -0.000000000000000, 1.767766952966368, -0.000000000000001, 1.767766952966370, 2.500000000000000, 1.767766952966369, 0.000000000000001, -1.767766952966369, -2.500000000000000, -1.767766952966369};
    
    float v[2] = {0,0};
    
    //std::cout << std::setprecision(15) << std::fixed;
    
    for (int i = 0; i<number_mics; i++){
        v[0] += pinv[0][i] * (float)delays[i];
        v[1] += pinv[1][i] * (float)delays[i];
    }
    
    // Normalize resulting vector
    float norm_v = sqrt(v[0]*v[0]+v[1]*v[1]);
    if (norm_v == 0) norm_v = 0.000000000000000000001;
    v[0] /= norm_v;
    v[1] /= norm_v;
    
    //std::cout << "\n v = ( " << v[0] << ", " << v[1] << " )" << std::endl ;
    
    // Determine angle
    float val_angle = 0;
 //   val_angle = atan (v[1]/v[0]);
 //   if (acos(v[0]) < PI/2) val_angle = asin(v[1]); // the vector v is already normalized
 //   else val_angle = PI-asin(v[1]);
    
    if (v[0] >= 0){ // the vector v is already normalized
        val_angle = asin(v[1]);
    }else{
        if (v[1] >= 0) val_angle = PI - asin(v[1]);
        else val_angle = -PI - asin(v[1]);
    }
    //std::cout << "This is the angle =" <<val_angle<< std::endl;
    results->angle = val_angle ;

    //std::cout << "\n val_angle = " << val_angle << std::endl ;
    
// ***
// ************************************************************************************************************************

    /*
  //  /!\ For testing purposes only /!\  Create a sine wave/ramp to send to gui.
  static float phase = 0 ;
  //float val = 0.5 + 0.6 * sin(phase) ;
  phase += .05 ;
  if ( phase > PI )
    phase -= 2*PI ;
  float val = phase ;
  //results->angle = val ;
    
    
  // ------ Do an FFT ------
  //int Nfft = 64*16 ;
  kiss_fft_cfg cfg = kiss_fft_alloc( Nfft, 0, 0, 0 );
  kiss_fft_cpx cx_in[Nfft];
  kiss_fft_cpx cx_out[Nfft];

  // put kth sample in cx_in[k].r and cx_in[k].i
  int channel = 0;
  for (int i = 0; i< Nfft; i++){
    cx_in[i].r = buf[ i * Nch + channel ] ;
    cx_in[i].i = 0.0 ;
  }
  kiss_fft( cfg , cx_in , cx_out );

  // transformed. DC is in cx_out[0].r and cx_out[0].i
  for (int i = 0; i< Nfft/16; i++){
    results->spec[i] = sqrt( cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i ) ; // abs(...)
    results->spec[i] *= 1.0 / (float)Nfft ;
  }
  results->Nspec = Nfft/16 ;

  free(cfg);
     */

     
  // ------ Send the results ------

  send_results( results ) ;


  // ------ finish processing? ------
  sample_count += Nsamples ;
  if ( limit_sample_number & ( sample_count > sample_limit_number ) ) {
    processing_finished = true ;
    return ;
  }
};


void process::set_sample_rate( float val ) { sample_rate = val ; } ;

void process::set_channels( int val ) { channels = val ; } ;


void process::pre_start() {
  std::cout << "process::pre_start()" << std::endl ;
  open_snd_file();
} ;

void process::post_stop() {
  std::cout << "process::post_stop()" << std::endl ;
  close_snd_file();
} ;



void process::open_snd_file() {
  SF_INFO info;
  info.format=SF_FORMAT_WAV | SF_FORMAT_PCM_16 ;
  //info.format=SF_FORMAT_WAV | SF_FORMAT_PCM_24 ;
  //info.format=SF_FORMAT_WAV | SF_FORMAT_FLOAT ;
  info.channels = channels ;
  info.samplerate = sample_rate ;
  //const char* outfilename = "foo2.wav" ;

  //infile =  sf_open(outfilename, SFM_WRITE, &info ) ;
  infile =  sf_open( outfilename.c_str(), SFM_WRITE, &info ) ;
  if ( not infile ) {
    std::cout << "Cannot open sound file: " << outfilename << std::endl ;
  }
};


void process::close_snd_file() {
  if ( sf_close( infile ) ) {
    std::cout << "Error closing sound file." << std::endl ;
  };
} ;



float process::get_level( int c) {
  if (!levels) return 0.0f ;
  if ( (c < 0) | (c >= channels) ) return 0.0f ;
  pthread_mutex_lock( &mutex_levels ) ;
  float val = levels[c] ;
  pthread_mutex_unlock( &mutex_levels ) ;
  return val ;
};



void process::set_samples_to_process( int count ){
  if ( count <= 0 ) {
    limit_sample_number = false ;
  } else {
    sample_limit_number = count ;
    limit_sample_number = true ;
  }
}


void process::send_results( processed_data* d ){

  if ( rb_results.free_space() ) {
    rb_results.put( (void**)&d, 1 ) ;
  } else {
    std::cout << "rb_results full!!" << std::endl ;
    delete d;
  }

};



processed_data* process::get_result() {
  if ( rb_results.available()  ) {
    processed_data* r ;
    rb_results.get( (void**)&r, 1 ) ;
    return r;
  }
  return 0;
};


