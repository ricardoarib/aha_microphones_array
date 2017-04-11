

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>
#include <getopt.h>
#include "pthread.h"

#include <bitset>
#include <cstdlib>

#include "process.h"
#include "ascii_meters.h"
#include "user_interface.h"

#define PI 3.14159265358979


void* ui_thread(void * context){
  ((user_interface *)context)->go();
  return 0 ;
};


void version() {
  std::cout << "version 0.1.0" << std::endl ;
}

void help_cmdl( char** argv ) {
  std::cout << "Usage:" << std::endl ;
  std::cout << argv[0] << " [options] <soundfile>" << std::endl ;
  version() ;
  std::cout << "arguments:        " << std::endl ;
  std::cout << "    <soundfile>            - filename of the file containing the input sound data." << std::endl ;
  std::cout << "options:        " << std::endl ;
  std::cout << "    -o <soundfile>         - filename of the file where to save the audio ( default: out.wav )." << std::endl ;
  std::cout << "    -d <delay>             - delay (in miliseconds) between process callback calls. Slows down the simulation. (default = 0.0)" << std::endl ;
  std::cout << "    -r                     - Slows down simulation to a speed similar to real time. this overides the -d option." << std::endl ;
  std::cout << "    -h                     - show this help." << std::endl ;
  std::cout << "    -v                     - show software version." << std::endl ;
  std::cout << "    " << std::endl;
}


int main( int argc, char** argv ) {


  // ------ Parse command line options. ------

  std::string filename_in ;
  std::string filename_out ;
  float delay_ms = 0 ;
  bool realtime = false ;

  int opt;
  while ( ( opt = getopt(argc,argv,"hvro:d:") ) != -1 ) {
    switch ( opt ) {
    case 'o' :
      filename_out.assign( optarg ) ;
      break ;
    case 'd' :
      delay_ms = atof( optarg ) ;
      break ;
    case 'r' :
      realtime = true ;
      break ;
    case 'h' :
      help_cmdl( argv ) ;
      return 0 ;
      break ;
    case 'v' :
      version() ;
      return 0 ;
      break ;
    default :
      help_cmdl( argv ) ;
      return -1 ;
    }
  }
  if ( argc != optind+1 ) {
    help_cmdl( argv ) ;
    return -1 ;
  }

  filename_in.assign( argv[optind] ) ;

  if ( filename_out.empty() )
    filename_out = std::string( "out.wav" ) ;


  std::cout << "Reading from file: " << filename_in << std::endl ;
  std::cout << "Saving to file:    " << filename_out << std::endl ;
  if ( realtime )
    std::cout << "Slowed down to realtime speed." << std::endl ;
  else
    std::cout << "Delay between callback calls:    " << delay_ms << " miliseconds" << std::endl ;


  int Nsamples = 1024 ;

  // ------ prepare input sound file ------

  SNDFILE* infile = 0 ;
  SF_INFO info ;

  info.format = 0 ;
  infile =  sf_open( filename_in.c_str(), SFM_READ, &info ) ;
  if ( not infile ) {
    std::cout << "Cannot open sound file: " << filename_in << std::endl ;
    return -1 ;
  }

  /*
  std::cout << "  info.frames=" << info.frames ;
  std::cout << "  info.channels=" << info.channels ;
  std::cout << "  info.samplerate=" << info.samplerate ;
  std::cout << "  info.format=" << std::bitset<32>(info.format) ;
  //std::cout << "  info.=" << info. ;
  //std::cout << "  info.=" << info. ;
  std::cout << std::endl ;
  */

  int channels = info.channels ;

  float buf[ Nsamples * channels ] ;


  // ------ determine delay value ------

  useconds_t sleep_time_us ;
  if ( realtime ) {
    sleep_time_us = (float)Nsamples / info.samplerate * 1000000.0 ;
  } else {
    sleep_time_us = delay_ms * 1000 ;
  }

  // ------ Prepare all classes. ------

  process p( filename_out ) ;
  user_interface ui( &p ) ;


  // ------ Start user interface thread --------

  pthread_t ui_thread_id ;
  pthread_create( &ui_thread_id, NULL, ui_thread, (void*) &ui ) ;


  // ------ Init  ------
  p.set_sample_rate( info.samplerate ) ;
  p.set_channels( channels ) ;
  p.pre_start() ;


  // ------ Loop  ------
  int read_count ;
  //int count = 0 ;
  while ( read_count = sf_readf_float(infile,buf,Nsamples)  ) {
    // std::cout << "count = " << count++ << "  read_count = " << read_count << "   Nsamples = " << Nsamples << std::endl ;
    p.callback( buf, channels, read_count ) ;
    usleep( sleep_time_us ) ;
  } ;


  // ------ Finish ------
  p.post_stop() ;

  if ( sf_close( infile ) ) {
    std::cout << "Error closing sound file." << std::endl ;
  };

  // ------ Stop user interface thread -------

  pthread_cancel( ui_thread_id ) ; // Very ugly!! But works. Good enough for now.

  std::cout << std::endl ;

  return 0 ;

  //return ui.go() ;


}





