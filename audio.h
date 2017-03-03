#include "portaudio.h"

class audio{
 public:
  audio();
  ~audio();
  int list_devices_info();
  void open_device();

  int get_count(){ return count; };
  int get_num_channels(){ return num_channels; };
  float get_level(int c);

  int callback( const void *inputBuffer, void *outputBuffer,
	    unsigned long framesPerBuffer,
	    const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);

 private:
  bool pa_init, pa_open, pa_streamming;
  int count;
  int num_channels;
  float* levels;
};
