

#include <iostream>

#include "audio.h"


int main(int argc, char** argv){
  audio a;
  a.list_devices_info();
  a.open_device();
  int count = 0;

  while ( count < 100 ){
    count = a.get_count();
    std::cout << "\rcount = " << count << "         " << std::flush ;
  }
  return 0;
}

