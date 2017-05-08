
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


void createSinusoidSignal (int Npoints, double * buffer){
    double rate, result;
    
    //std::cout << "begin" << std::endl;
    for (int i = 0; i<Npoints; i++){
        rate = 5.0 * i;
        result = cos (rate * PI/180);
        buffer [i] = result;
        std::cout << "signal [" << i << "]: " << buffer [i] << std::endl;
        //std::cout << result<< std::endl;
    }
    //std::cout << "end" << std::endl;
}


// Delay Calculation
void delayCalculation (float * spec_ref, float * spec_signal, float * spec_result, int Nfft, int fs, float * delay, int * max_idx){
    float num_spec_result, den_spec_result;
    float spec_ref_abs, spec_signal_abs;
    
    for (int i = 0; i < Nfft; i++){
        num_spec_result = spec_ref[i] * spec_signal[i];
        
        spec_ref_abs = (spec_ref[i]<0) ? -spec_ref[i] : spec_ref[i];
        spec_signal_abs = (spec_signal[i]<0) ? -spec_signal[i] : spec_signal[i];
        den_spec_result = spec_ref_abs * spec_signal_abs;
        
        spec_result[i] = num_spec_result/ den_spec_result;
    }
    
    
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

    // Store spectrum
    
    /*
    int Nfft = 64*16 ;
    float spectrums[Nch][Nfft/16];
    kiss_fft_cfg cfg = kiss_fft_alloc( Nfft, 0, 0, 0 );
    kiss_fft_cpx cx_in[Nfft];
    kiss_fft_cpx cx_out[Nfft];
    
    for (int channel = 0; channel < Nch; channel++){
        for (int i = 0; i< Nfft; i++){
            cx_in[i].r = buf[ i * Nch + channel ] ;
            cx_in[i].i = 0.0 ;
        }
        kiss_fft( cfg , cx_in , cx_out );
        for (int i = 0; i< Nfft/16; i++){
            spectrums[channel][i] = sqrt( cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i ) ; // abs(...)
            spectrums[channel][i] *= 1.0 / (float)Nfft ;
        }
    }
    free(cfg);
    float spec[64];
    
    float signal[1024];
    for (int i = 0; i < Nsamples; i++){
        signal[i] = signals[1][i];
    }
    
    *spec = calcSpectrum (signal, Nfft);
     
     */
    
    
    // Define reference spectrum
  /*  int ref_channel = 0;
    float ref_spectrum [Nfft/16];
    for (int i = 0; i < Nfft/16; i++){
        ref_spectrum [i] = spectrums[ref_channel][i];
    }
   */

    // Generalized cross correlation
//    int Nfft = 64*16;
//    float num_specs [Nch][Nfft/16];
//    float den_specs [Nch][Nfft/16];
//    float xcorr [Nch][Nfft/16];
    
    /*
    for (int channel = 0; channel < Nch; channel++){
        for (int i; i < Nfft/16; i++){
            num_specs [channel][i] =
            den_specs [channel][i] =
            xcorr [channel][i] =
        }
    
    }*/
    /*
   
     
     
     
     for (int channel = 0; channel < Nch; channel++){
     for(int i; i < 64; i++){
     nspecs [channel][i] = ref_spec [channel][i] * specs[channel][i];
     dspecs [channel][i] = sqrt( ref_spec[channel][i] * ref_spec[channel][i] ) * sqrt ( specs[channel][i] * specs[channel][i] );
     xcorr [channel][i] = nspecs [channel][i] / dspecs[channel][i];
     //    myfile << "Writing this to a file.\n" << endl;
     }
     }
     //   myfile.close();
     
    // reverse FFT needed (!!!)
    std::cout << "-------16------" << std::endl ;
    
    //[~, I] = max(abs(xc));
    float xcorr_abs [10][64] = {0};
    for (int channel = 0; channel < Nch; channel++){
        for(int i; i < 64; i++){
            xcorr_abs [channel][i] = sqrt (xcorr [channel][i] * xcorr [channel][i]);
        }
    }
    std::cout << "-------16------" << std::endl ;
    

     */
    
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
    int Npts = 64;
    double buffer[Npts];
    
    createSinusoidSignal(Npts, &buffer[0]);
    
    int Nfft = 64;
    float buf_spec[Nfft];
    
    kiss_fft_cfg cfg = kiss_fft_alloc ( Nfft, 0, 0, 0);
    kiss_fft_cpx cx_in [Nfft]; //ou cx_in[Npts] ???
    kiss_fft_cpx cx_out [Nfft];
    
    for (int i = 0; i<Npts; i++){
        cx_in[i].r = buffer[i];
        cx_in[i].i = 0.0;
    }
    kiss_fft (cfg , cx_in, cx_out);
    
    //std::cout << "begin" << std::endl ;
    for (int i = 0; i<Nfft; i++){
        buf_spec[i] = sqrt( cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i );
        buf_spec[i]  *= 1.0 / (float)Nfft ;
        //std::cout <<buf_spec[i] << std::endl ;
    }
    free(cfg);
    
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
    
    std::cout << "begin" << std::endl ;
    for (int i = 0; i<Nfft; i++){
        //i_buffer[i] =(float) sqrt( i_cx_out[i].r * i_cx_out[i].r);
        i_buffer[i] = sqrt( i_cx_out[i].r * i_cx_out[i].r + i_cx_out[i].i * i_cx_out[i].i );
        i_buffer[i]  *= 1.0 / (float)Nfft ;
        std::cout <<"i_buffer["<<i<<"]: "<< i_buffer[i] << std::endl ;
        //std::cout << i_buffer[i] << std::endl ;
        //std::cout <<"cx_out: "<<cx_out[i].r<< std::endl ;
    }
    free(i_cfg);
    std::cout << "done" << std::endl ;
    
    
    
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


