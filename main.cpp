
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
  a.open_device();

  if (!a.is_streamming())
    return -1;
  std::cout << "cpu %  | count |   vu meters (dBFS) \n" ;
  int count = 0;
  double cpu;
  while ( count < 100 ){
    count = a.get_count();
    cpu = a.get_cpu_load();
    //std::cout << "\rcount = " << count << "         " ;
    std::cout << "\r" << std::setprecision(2) << std::setw(6) << cpu *100 << " | " << std::setw(5) << count << " | " ;
    for (int c=0; c < a.get_num_channels(); c++){
      float l = a.get_level(c) ;
      //std::cout.precision(4);
      //std::cout << std::scientific << l << "  "  ;
      print_bar(l);
      //std::cout << " " ;
    }
    std::cout << std::flush ;
  }
  return 0;
}

