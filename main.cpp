
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>

#include "audio.h"
#include "process.h"


#define PI 3.14159265358979

void print_bar(float val, float& max, int& cnt){ /*!< Value*/
  #define START_CNT 25
  if ( val >= max ) {
    max = val ;
    cnt = START_CNT ;
  } else {
    cnt-- ;
  }

  if ( cnt <= 0 ){
    max = val ;
    cnt = START_CNT ;
  }

  int N =  8; // bar total length
  float max_dB = 0.0f;
  float min_dB = -40.0f;
  float val_dB = 20*log10(val) ;
  float val_max_dB = 20*log10(max) ;

  //int n = N+val_log*4*N ;
  int n     = ( val_dB     - min_dB)/(max_dB-min_dB) * N;
  int n_max = ( val_max_dB - min_dB)/(max_dB-min_dB) * N;

  //std::cout.precision(0);
  std::cout << std::setw(5) << std::setprecision(1) ;
  //std::cout << std::fixed << " " << val_dB << "dBFS" ;
  std::cout << std::fixed << val_dB ;

  if ( n > N )
    n = N;
  if ( n < 0 )
    n = 0;

  if ( n_max > N )
    n_max = N;
  if ( n_max < 0 )
    n_max = 0;
  /*
  for (int i=0; i< n; i++)
    std::cout << '#';
  for (int i=n; i< N; i++)
    std::cout << ' ';
  */
  for (int i=0; i< N; i++) {
    if ( i == n_max )
      std::cout << '!';
    else if ( i <= n )
      std::cout << '#';
    else
      std::cout << ' ';
  }
};



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
      //print_bar( levels[c] ) ;
      print_bar( a.get_level(c), levels_max[c], levels_counts[c] ) ;
    }
    //std::cout << std::endl ;
    //std::cout << std::endl ;
    //std::cout << " fpb=" << a.get_framesPerBuffer() << std::endl ;
    std::cout << " fpb=" << a.get_framesPerBuffer() ;
    std::cout << std::flush ;


    usleep( sleep_time_us ) ;

  }


  return 0;
}

