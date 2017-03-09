
#include <iostream>
#include <iomanip>
#include <cmath>

#include "audio.h"


void print_bar(float val){ /*!< Value*/
  int N =  8; // bar total length
  float max_dB = 0.0f;
  float min_dB = -40.0f;
  float val_dB = 20*log10(val) ;

  //int n = N+val_log*4*N ;
  int n = (val_dB-min_dB)/(max_dB-min_dB) * N;

  //std::cout.precision(0);
  std::cout << std::setw(5) << std::setprecision(1) ;
  //std::cout << std::fixed << " " << val_dB << "dBFS" ;
  std::cout << std::fixed << val_dB ;

  if ( n > N )
    n = N;
  if ( n < 0 )
    n = 0;
  for (int i=0; i< n; i++)
    std::cout << '#';
  for (int i=n; i< N; i++)
    std::cout << ' ';
};



int main(int argc, char** argv){
  audio a;
  a.list_devices_info();
  //a.open_device();
  a.start("STM32");

  int num_samples = 1024*4;
  int num_channels = a.get_num_channels();

  float buf[ num_samples * num_channels ];
  for (int i =0; i<num_samples * num_channels; i++)
    buf[i] = 0.0f ;
  
  float levels[ num_channels ];
  /*  for (int c=0; c<num_channels; c++ ) {
    levels[c] = 0.0f ;
  }
  */
  if (!a.is_streamming())
    return -1;
  std::cout << "cpu %  | count |   vu meters (dBFS) \n" ;
  int count = 0;
  double cpu;
  while ( count < 10000 ){
    count = a.get_count();
    cpu = a.get_cpu_load();
    //std::cout << "\rcount = " << count << "         " ;
    std::cout << "\r" << std::setprecision(2) << std::setw(6) << cpu *100 << " | " << std::setw(5) << count << " | " ;
    /*
    for (int c=0; c < a.get_num_channels(); c++){
      float l = a.get_level(c) ;
      //std::cout.precision(4);
      //std::cout << std::scientific << l << "  "  ;
      print_bar(l);
      //std::cout << " " ;
    }
    std::cout << std::flush ;
    */


    /* decay levels */
    for (int c=0; c<num_channels; c++ ) {
      levels[c] *= 0.09f ;
    }
    
    int read = a.get_data( buf, num_samples * num_channels ) ;

    if ( !read )
      continue;
    
    float* rptr = buf;
    
    for(int i=0; i<num_samples; i++ ) {
      for (int c=0; c<num_channels; c++ ) {
	float sample = *rptr++;
	float sample_abs = (sample<0) ? -sample : sample ;
	if ( levels[c] < sample_abs )
	  levels[c] = sample_abs ;
      }
    }

    //std::cout << "\n";
    for (int c=0; c<num_channels; c++ ) {
      print_bar( levels[c] ) ;
    }
    //std::cout << std::endl ;
    //std::cout << std::endl ;
    std::cout << std::flush ;


    
    
  }
  return 0;
}

