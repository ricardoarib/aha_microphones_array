
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

// Create cosine file


void createSinusoidSignal (int Npoints, float * buffer){
    float rate, result;
    
    for (int i = 0; i<Npoints; i++){
        rate = 10.0 * i;
        result = cos (rate * PI/180);
        buffer [i] = result;
    }
}

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


float abs (float value){
    value = sqrt(value * value);
    return value;
}

int findMaxIdx (float * signal, int signal_length){
    
    int max_idx = 0;
    
    for (int i = 0; i < signal_length; i++){
        
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
        xc[i] = sqrt( i_cx_out[i].r * i_cx_out[i].r + i_cx_out[i].i * i_cx_out[i].i );
        xc[i]  *= 1.0 / (float)Nfft ;
    }
    free(i_cfg);

    int max_idx = 0;
    
    max_idx = findMaxIdx(&xc[0], Nfft);
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

    
    // Store signals
    float signals [Nch][Nsamples];
    for (int channel = 0; channel < Nch; channel++){
        for (int i = 0; i < Nsamples; i++){
            signals [channel][i] = buf [channel + i*Nch];
        }
    }

    
// ***
// ************************************************************************************************************************

    
  //  /!\ For testing purposes only /!\  Create a sine wave/ramp to send to gui.
  static float phase = 0 ;
  //float val = 0.5 + 0.6 * sin(phase) ;
  phase += .05 ;
  if ( phase > PI )
    phase -= 2*PI ;
  float val = phase ;
  results->angle = val ;
    
    
    // ::::::::::::::::::::::::::::::::::::::::::
    int Npts = 1024;
    float buffer[Npts];
    float delayed_buffer[Npts];
    
    createSinusoidSignal(Npts, &buffer[0]);
    addDelayToSignal (Npts, &buffer[0], &delayed_buffer[0], 100, 1);
    //for (int i=0; i<Npts; i++){
    //    std::cout << "Signal_delayed["<<i<<"]: "<<delayed_buffer[i]<< std::endl;
    //}
    
    //void fft (int Nfft, float * signal, float * result_fft){
    int Nfft = 1024;
    float result_fft_RE[Nfft], result_fft_IM[Nfft];

    fft(Nfft, &buffer[0], &result_fft_RE[0], &result_fft_IM[0]);
    
    // FFT SIG 2
    float result_fft2_RE[Nfft], result_fft2_IM[Nfft];
    fft(Nfft, &delayed_buffer[0], &result_fft2_RE[0], &result_fft2_IM[0]);
    
    float delay_result = 0;
    int max_idx = 0;

    
    //int xcorr (int Nfft, float * spec1_RE, float * spec1_IM, float * spec2_RE, float * spec2_IM, int fs){

    int fs = 1; // DO NOT FORGET TO CHANGE THIS!
    max_idx = xcorr(Nfft, &result_fft_RE[0], &result_fft_IM[0], &result_fft2_RE[0], &result_fft2_IM[0], fs);
    //std::cout << "max_idx: "<< max_idx<< " para fs = "<<fs<< std::endl;
    delay_result = max_idx/fs;
    //std::cout << "delay result: "<< delay_result<< " para fs = "<<fs<< std::endl;
    
    // int findMaxIdx (float * signal, int signal_length){
    //max_idx = findMaxIdx(buf_spec, Nfft);
    //std::cout << "MAX_IDX: "<< max_idx << std::endl;
    
    /*
    
    //std::cout << "done" << std::endl ;
    kiss_fft_cfg i_cfg = kiss_fft_alloc( Npts, 1, NULL, NULL);
    kiss_fft_cpx i_cx_in [Nfft];
    kiss_fft_cpx i_cx_out [Nfft];
    
    double i_buffer[Npts];

    for (int i = 0; i<Npts; i++){
        i_cx_in[i].r = buf_spec[i];
        i_cx_in[i].i = 0.0;
        //i_cx_in[i] = buf_spec[i];
    }
    //kiss_fft (i_cfg , i_cx_in, i_cx_out);
    kiss_fft (i_cfg , cx_out, i_cx_out);
    //kiss_fft (i_cfg , (kiss_fft_cpx*)&buf_spec[0], (kiss_fft_cpx*)&i_buffer[0]);
    
    //std::cout << "begin" << std::endl ;
    for (int i = 0; i<Nfft; i++){
        //i_buffer[i] =(float) sqrt( i_cx_out[i].r * i_cx_out[i].r);
        i_buffer[i] = sqrt( i_cx_out[i].r * i_cx_out[i].r + i_cx_out[i].i * i_cx_out[i].i );
        i_buffer[i]  *= 1.0 / (float)Nfft ;
        //std::cout <<"i_buffer["<<i<<"]: "<< i_buffer[i] << std::endl ;
        //std::cout << i_buffer[i] << std::endl ;
        //std::cout <<"cx_out: "<<cx_out[i].r<< std::endl ;
    }
    free(i_cfg);
    //std::cout << "done" << std::endl ;
    */
    
    // ::::::::::::::::::::::::::::::::::::::::::
    
    
    /*
  // ------ Do an FFT ------
  int Nfft = 64*16 ;
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


