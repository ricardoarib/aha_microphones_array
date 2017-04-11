

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>
#include <getopt.h>

#include "audio.h"
#include "process.h"
#include "ascii_meters.h"
#include "user_interface.h"

#define PI 3.14159265358979

void version() {
  std::cout << "version 0.1.0" << std::endl ;
}

void help_cmdl( char** argv ) {
  std::cout << "Usage:" << std::endl ;
  std::cout << argv[0] << " [options]" << std::endl ;
  version() ;
  std::cout << "options:        " << std::endl ;
  std::cout << "    -d <device_name>       - defines the name of the device to use ( default: STM32 )." << std::endl ;
  std::cout << "    -o <sound_filename>    - defines the file where to save the audio ( default: out.wav )." << std::endl ;
  std::cout << "    -h                     - show this help." << std::endl ;
  std::cout << "    -v                     - show software version." << std::endl ;
  std::cout << "    " << std::endl;
}


void list_devices() {
  audio a ;
  a.list_devices_info() ;
}


int main( int argc, char** argv ) {


  // ------ Parse command line options. ------

  std::string device ;
  std::string filename ;

  int opt;
  while ( ( opt = getopt(argc,argv,"hvld:o:") ) != -1 ) {
    switch ( opt ) {
    case 'd' :
      device.assign( optarg ) ;
      break ;
    case 'o' :
      filename.assign( optarg ) ;
      break ;
    case 'h' :
      help_cmdl( argv ) ;
      return 0 ;
      break ;
    case 'v' :
      version() ;
      return 0 ;
      break ;
    case 'l' :
      list_devices() ;
      return 0 ;
      break ;
    default :
      help_cmdl( argv ) ;
      return -1 ;
    }
  }
  if ( argc != optind ) {
    help_cmdl( argv ) ;
    return -1 ;
  }

  if ( device.empty() )
    device = std::string( "STM32" ) ; // other examples: "default"  "HDA"

  if ( filename.empty() )
    filename = std::string( "out.wav" ) ;

  std::cout << "Using device:   " << device << std::endl ;
  std::cout << "Saving to file: " << filename << std::endl ;



  // ------ Prepare all classes. ------

  audio a ;
  process p( filename ) ;
  user_interface ui( &p ) ;

  if ( a.open_device( device.c_str() ) ) {
    std::cout << "ERROR: Cannot open audio device." << std::endl ;
    return -1 ;
  }

  a.set_audio_proc( &p, 1024 ) ;

  // ------ Start! ------

  a.start() ;

  if ( !a.is_streamming() ) {
    std::cout << "Ooops! Portaudio is not streamming (but it should)." << std::endl ;
    return -1 ;
  }


  // ------ Goto the user interface. ------

  int count = a.get_count() ;
  float cpu = a.get_cpu_load() ;

  std::cout << std::setprecision(2) << std::setw(6) << cpu *100 << " | " << std::setw(5) << count << " | " ;

  std::cout << " fpb=" << a.get_framesPerBuffer() ;
  std::cout << std::endl ;


  return ui.go() ;


}

