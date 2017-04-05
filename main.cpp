
#include <iostream>
#include <iomanip>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>

#include "audio.h"
#include "process.h"


#define PI 3.14159265358979

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
  process p;
  audio a;
  a.list_devices_info();
  //a.open_device();


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


  SF_INFO info;
  info.format=SF_FORMAT_WAV | SF_FORMAT_PCM_16 ;
  //info.format=SF_FORMAT_WAV | SF_FORMAT_FLOAT ;
  info.channels = 10 ;
  info.samplerate = a.get_sample_rate() ;
  const char* outfilename = "foo.wav" ;


  
  SNDFILE* infile =  sf_open(outfilename, SFM_WRITE, &info ) ;
  if ( not infile ) {
    std::cout << "Cannot open sound file: " << outfilename << std::endl ;
    return -1 ;
  }


  float t = 0 ;
  float T = 1/a.get_sample_rate();
  
  
  std::cout << "cpu %  | count |   vu meters (dBFS) \n" ;
  int count = 0;
  double cpu;
  while ( count < 3 ){
    sleep(1);
    count++;
    continue ;

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
    std::cout << " fpb=" << a.get_framesPerBuffer() << std::endl ;
    std::cout << std::flush ;


    /*
    rptr = buf;
    for(int i=0; i<num_samples; i++ ) {
      t += T ;
      for (int c=0; c<num_channels; c++ ) {
	if (c==1)
	  *rptr  = sin( 2.0 * PI * 1000.0 * t  ) ;
	rptr++;
      }
    }
    */


    sf_write_float( infile, buf, num_samples * num_channels ) ;
    
    
  }



  
  if ( sf_close( infile ) ) {
    std::cout << "Error closing sound file." << std::endl ;
  };
  
  return 0;
}

