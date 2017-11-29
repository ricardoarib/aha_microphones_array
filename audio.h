#ifndef audio_h
#define audio_h


#include "portaudio.h"
#include "ringbuffer.h"
#include "audio_proc.h"
#include "pthread.h"

//! Audio I/O class.
/*!
A class that reads from and writes to an audio device.
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
  int list_devices_info( bool show_latencies = false );
  PaDeviceIndex search_device(const char* str);

  /// \brief Opens the audio device and starts streamming.
  /// \param devname Optional string to specify the device. If the string is not specified then uses the default device.
  void start();
  int  open_device( const char* devname = 0 );
  int  open_device( PaDeviceIndex d );
  void start_stream();

  int get_count(){ return count; };
  int get_num_channels(){ return num_channels; };
  double get_sample_rate(){ return sample_rate; };
  int is_streamming(){ return pa_streamming; };
  float get_level(int c);

  int callback( const void *inputBuffer, void *outputBuffer,
	    unsigned long framesPerBuffer,
	    const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);


  double get_cpu_load();   /*!< Gets how much CPU is being used by the callback (returns value between 0.0 and 1.0)*/

  int get_data(float* d, int sz) ;
  unsigned long get_framesPerBuffer(){ return _framesPerBuffer ; } ;

  // void set_audio_proc(audio_proc* audiop, int buf_length = 1024 ) ; 
  void set_audio_proc(audio_proc* audiop, int buf_length ) ; 
  void set_samples_per_call( int val ){ samples_per_call = val;  };

  
 private:
  bool pa_init, pa_open, pa_streamming;
  PaStream* pa_stream;

  int count;
  int num_channels;
  double sample_rate;
  float* levels;

  ringbuffer<float> input_ring_buf ;
  
  unsigned long _framesPerBuffer;

  audio_proc* ap;

  ////////////////////////////////////////////
  /////////// callback_thread ////////////////
  ////////////////////////////////////////////
  static void* callback_thread_helper(void *);
  void* callback_thread();
  void start_callback_thread();
  void stop_callback_thread();

  pthread_t proc_thread_id;
  bool stop_proc_thread;
  bool thread_running;
  int callback_thread_sleep_time_us;
  int samples_per_call;

};



#endif
