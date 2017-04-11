
#include "process.h"
#include <iostream>

process::process( std::string fn ) :
  sample_rate( 1 ),
  channels( 1 ),
  outfilename( fn ),
  count( 0 )
  //levels( 0 )
{
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

  // write to wav file
  sf_write_float( infile, buf, Nsamples * Nch ) ;

};


void process::set_sample_rate( float val ) { sample_rate = val ; } ;

void process::set_channels( int val ) { channels = val ; } ;


void process::pre_start() {
  open_snd_file();
} ;

void process::post_stop() {
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
