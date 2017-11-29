

#include <iostream>
#include <iomanip>
#include <cmath>
#include <unistd.h>
#include <getopt.h>
#include <cstdlib>

#include "audio.h"
#include "process.h"
#include "ascii_meters.h"
#include "user_interface.h"

#define PI 3.14159265358979

#define DEFAULT_NUMBER_OF_SAMPLES 1024

void version() {
  std::cout << "version 0.1.0" << std::endl ;
}

void help_cmdl( char** argv ) {
  std::cout << "Usage:" << std::endl ;
  std::cout << argv[0] << " [options]" << std::endl ;
  version() ;
  std::cout << "options:" << std::endl ;
  std::cout << "    -d <device_name>       - defines the name of the device to use ( default: STM32 )." << std::endl ;
  std::cout << "    -l                     - lists all available audio devices." << std::endl ;
  std::cout << "    -o <sound_filename>    - defines the file where to save the audio ( default: out.wav )." << std::endl ;
  std::cout << "    -n <number>            - Stop processing after <number> samples. By default it never stops. May be overriden by -t option.(*)"  << std::endl ;
  std::cout << "    -t <seconds>           - Stop processing after <seconds>. By default it never stops. Overrides -n option.(*)"  << std::endl ;
  std::cout << "    -N <number>            - Sets the number of samples per callback. (default = " << DEFAULT_NUMBER_OF_SAMPLES << ")" << std::endl ;
  std::cout << "    -h                     - show this help." << std::endl ;
  std::cout << "    -v                     - show software version." << std::endl ;
  std::cout << "    " << std::endl;
  std::cout << "notes:    " << std::endl;
  std::cout << "    The -n and -t options are not precise. The actual stopping time/samples may be larger and correspond to blocks of callback samples set by the -N option." << std::endl;
  std::cout << "    " << std::endl;
  std::cout << "    " << std::endl;
}


void list_devices() {
  audio a ;
  a.list_devices_info() ;
}


int main( int argc, char** argv ) {

  std::cout << "sizeof(bool)=" << sizeof(bool) << std::endl ;
  std::cout << "sizeof(int)=" << sizeof(int) << std::endl ;
  std::cout << "sizeof(float)=" << sizeof(float) << std::endl ;
  std::cout << "sizeof(double)=" << sizeof(double) << std::endl ;
  std::cout << "sizeof(void*)=" << sizeof(void*) << std::endl ;


  // ------ Parse command line options. ------

  std::string device ;
  std::string filename ;
  int Nsamples = DEFAULT_NUMBER_OF_SAMPLES ;
  int samples_to_process = -1 ;
  float time_to_process = -1 ;

  int opt;
  while ( ( opt = getopt(argc,argv,"hvld:o:N:n:t:") ) != -1 ) {
    switch ( opt ) {
    case 'd' :
      device.assign( optarg ) ;
      break ;
    case 'o' :
      filename.assign( optarg ) ;
      break ;
    case 'n' :
      samples_to_process = atoi( optarg ) ;
      break ;
    case 't' :
      time_to_process = atof( optarg ) ;
      break ;
    case 'N' :
      Nsamples = atoi( optarg ) ;
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
  std::cout << "Nsamples: "<< Nsamples << std::endl;

  if ( device.empty() )
    device = std::string( "STM32" ) ; // other examples: "default"  "HDA"

  if ( filename.empty() )
    filename = std::string( "out.wav" ) ;

  std::cout << "Using device:   " << device << std::endl ;
  std::cout << "Saving to file: " << filename << std::endl ;



  // ------ Prepare all classes. ------

  audio a ;
  if ( a.open_device( device.c_str() ) ) {
    std::cout << "ERROR: Cannot open audio device." << std::endl ;
    return -1 ;
  }

  float sample_rate = a.get_sample_rate();
  if ( time_to_process > 0 ) {
    samples_to_process = time_to_process * sample_rate ;
  }
  if ( samples_to_process != -1 )
    std::cout << "Will stop after reaching " << samples_to_process << " samples or " << (float)samples_to_process / sample_rate << " seconds." << std::endl ;

  process p( filename, samples_to_process , Nsamples) ;
  user_interface ui( &p ) ;

std::cout << "Does is get here? samples_to_process: "<< samples_to_process << std::endl;
std::cout << "Does is get here? Nsamples: "<< Nsamples << std::endl;

  a.set_audio_proc( &p, Nsamples ) ;

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

