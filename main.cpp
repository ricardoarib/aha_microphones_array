
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>

#include "audio.h"
#include "process.h"
#include "ascii_meters.h"

#define PI 3.14159265358979



int main(int argc, char** argv){
  process p;
  audio a;
  a.list_devices_info();

  if ( a.open_device("STM32") ) {
  //if ( a.open_device("default") ) {
  //if ( a.open_device("HDA") ) {
    std::cout << "ERROR: Cannot open audio device." << std::endl ;
    return 1 ;
  }

  a.set_audio_proc(&p) ;

  a.start() ;


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

