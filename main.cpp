

#include <iostream>
#include <iomanip>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>
#include <getopt.h>

#include "audio.h"
#include "process.h"
#include "ascii_meters.h"

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

  audio a;
  process p( filename );


  if ( a.open_device( device.c_str() ) ) {
    std::cout << "ERROR: Cannot open audio device." << std::endl ;
    return -1 ;
  }

  a.set_audio_proc(&p) ;



  // ------ Start! ------

  a.start() ;



  // ------ User interface. ------


  int num_samples = 1024*4;
  int num_channels = a.get_num_channels();

  float levels_max[ num_channels ] ;
  int levels_counts[ num_channels ] ;
  for ( int i=0; i<num_channels; i++ ) {
    levels_max[i] = 0.0 ;
    levels_counts[i] = 0 ;
  }


  if (!a.is_streamming())
    return -1;

  
  std::cout << "cpu %  | count |   vu meters (dBFS) \n" ;
  float total_rec_time = 3; // seconds
  int count = 0;
  double cpu;
  double gui_rate = 25.0 ; // Hz
  useconds_t sleep_time_us = 1.0 / gui_rate * 1000000.0 ;
  //double sample_rate = a.get_sample_rate() ;

  while ( count < 10*1000 ) {

    count = a.get_count();
    cpu = a.get_cpu_load();
    //std::cout << "\rcount = " << count << "         " ;
    std::cout << "\r" << std::setprecision(2) << std::setw(6) << cpu *100 << " | " << std::setw(5) << count << " | " ;


    //std::cout << "\n";
    for (int c=0; c<num_channels; c++ ) {
      print_vertical_bar_dB( a.get_level(c) ) ;
    }
    for (int c=0; c<num_channels; c++ ) {
      //print_bar( levels[c] ) ;
      //print_bar( a.get_level(c), levels_max[c], levels_counts[c] ) ;
      print_db( a.get_level(c) ) ;
      //print_vertical_bar( a.get_level(c) ) ;
      //print_horizontal_bar( a.get_level(c) ) ;
      print_horizontal_bar( a.get_level(c), 3 ) ;
    }


    static float phase = 0;
    float val = 0.5 + 0.6 * sin(phase) ;
    phase += .05;
    print_horizontal_bar( val, 3 ) ;
    //print_vertical_bar( val ) ;
    //print_arrow( 360 * val ) ;
    print_needle( val, 20 ) ;


    //std::cout << std::endl ;
    //std::cout << " fpb=" << a.get_framesPerBuffer() << std::endl ;
    std::cout << " fpb=" << a.get_framesPerBuffer() ;
    std::cout << std::flush ;


    usleep( sleep_time_us ) ;

  }

  std::cout << std::endl ;

  return 0;
}

