#include "portaudio.h"


//! Audio I/O class.
/*!
A class that reads and writes to the audio device.
 */
class audio{
 public:
  /*!
    constructor
  */
  audio();
  ~audio();
  /*!
   Sarches devices and list their info.
  */
  int list_devices_info();
  PaDeviceIndex search_device(const char* str);
  void open_device();

  int get_count(){ return count; };
  int get_num_channels(){ return num_channels; };
  int get_sample_rate(){ return sample_rate; };
  int is_streamming(){ return pa_streamming; };
  float get_level(int c);

  int callback( const void *inputBuffer, void *outputBuffer,
	    unsigned long framesPerBuffer,
	    const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);

 private:
  bool pa_init, pa_open, pa_streamming;
  int count;
  int num_channels;
  int sample_rate;
  float* levels;

  

};
