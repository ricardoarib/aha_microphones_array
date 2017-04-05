
#include "process.h"
#include <iostream>

process::process() :
  sample_rate( 1 ),
  channels( 1 )
{

};


process::~process(){
};




void process::callback( float* buf, int Nch, int Nsamples ) {

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
  const char* outfilename = "foo2.wav" ;

  infile =  sf_open(outfilename, SFM_WRITE, &info ) ;
  if ( not infile ) {
    std::cout << "Cannot open sound file: " << outfilename << std::endl ;
  }
};


void process::close_snd_file() {
  if ( sf_close( infile ) ) {
    std::cout << "Error closing sound file." << std::endl ;
  };
} ;

