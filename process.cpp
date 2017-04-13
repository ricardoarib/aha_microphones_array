
#include "process.h"
#include <iostream>
#include <cmath>
#include "kiss_fft.h"

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

  //levels = new float[256] ;
  for (int i=0; i< 256; i++)
    levels[i] = 0 ;
};


process::~process(){
  //delete [] levels ;
};



void process::callback( float* buf, int Nch, int Nsamples ) {

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



  // ------ Has processing already finished? ------
  if ( processing_finished )
    return ;



  // ------ write to wav file ------
  sf_write_float( infile, buf, Nsamples * Nch ) ;


  // ------ allocate place to put the results ------
  processed_data* results = new processed_data ;



  //  /!\ For testing purposes only /!\  Create a sine wave/ramp to send to gui.
  static float phase = 0 ;
  //float val = 0.5 + 0.6 * sin(phase) ;
  phase += .05 ;
  if ( phase > PI )
    phase -= 2*PI ;
  float val = phase ;
  results->angle = val ;

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
  return levels[c] ;
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


