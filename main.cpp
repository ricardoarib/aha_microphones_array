

#include <iostream>

#include "audio.h"


int main(int argc, char** argv){
  audio a;
  a.list_devices_info();
  a.open_device();

  if (!a.is_streamming())
    return -1;
  
  int count = 0;
  while ( count < 10000 ){
    count = a.get_count();
    std::cout << "\rcount = " << count << "         " ;
    for (int c=0; c < a.get_num_channels(); c++){
      float l = a.get_level(c) ;
      std::cout << l << "  "  ;
    }
    std::cout << std::flush ;
  }
  return 0;
}

