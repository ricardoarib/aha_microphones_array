/*! \file */


#include "audio.h"

#include "portaudio.h"
#include <iostream>
#include <string>
//#include <cmath>
#include <unistd.h>


#define PI 3.14159265358979

#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
//#define SAMPLE_RATE  (16000)
//#define FRAMES_PER_BUFFER (512)
#define FRAMES_PER_BUFFER (128)


static int external_callback( const void *inputBuffer, void *outputBuffer,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
			      void *userData ) ;



bool string_search(const char* a, const char* b ){
  std::string s1(a);
  std::string s2(b);
  //return s1.find(s2) != std::string::npos ;
  return s1.find(s2) == 0 ;
};


audio::audio() :
  count( 0 ),
  pa_init( false ), pa_open( false ), pa_streamming( false ),
  pa_stream(0),
  num_channels( 0 ), sample_rate( 1 ),
  levels( 0 ),
  input_ring_buf( 1024*256 ),
  ap( 0 ),
  stop_proc_thread( false ),
  thread_running( false ),
  callback_thread_sleep_time_us( 1000 )
{


  levels = new float[256];
  PaError err;
  err = Pa_Initialize();


  if( err != paNoError ) {
    //printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    //return 1;
    goto error;
  };

  pa_init = true;

  return;
 error:

  return;

};


audio::~audio(){

  stop_callback_thread() ;

  PaError err ;

  if (pa_streamming){
    err = Pa_StopStream( pa_stream );
    if( err != paNoError ) {
      std::cout << "StopStream: PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
    };
  };


  if (pa_open){
    err = Pa_CloseStream( pa_stream );
    if( err != paNoError ) {
      std::cout << "CloseStream: PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
    };
  }


  if (pa_init) {
    err = Pa_Terminate();
    if( err != paNoError ) {
      std::cout << "Terminate: PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
    };
  }

  delete [] levels;
};



int audio::list_devices_info(){
  PaHostApiIndex numberAPIs = Pa_GetHostApiCount() ;
  std::cout << "\n" << "Pa_GetHostApiCount returned "<< numberAPIs << std::endl;
  const PaHostApiInfo * apiInfo ;
  for ( int i=0; i<numberAPIs; i++){
    apiInfo = Pa_GetHostApiInfo( i ) ;
    std::cout << "\n";
    std::cout << "hostAPI type:\t" << apiInfo->type << "\n";
    std::cout << "hostAPI:\t" << apiInfo->name << "\n";
    std::cout << "deviceCount:\t" << apiInfo->deviceCount << "\n";
    std::cout << "defaultInputDevice:\t" << apiInfo->defaultInputDevice << "\n";
    std::cout << "defaultOutputDevice:\t" << apiInfo->defaultOutputDevice << "\n";
  }


  int numDevices;
  numDevices = Pa_GetDeviceCount();
  if( numDevices < 0 )  {
    std::cout << "ERROR: Pa_CountDevices returned "<< numDevices << std::endl;
    //err = numDevices;
    //goto error;
    return -1;
  }

  std::cout << "\n\nPa_CountDevices returned "<< numDevices << std::endl;

  const   PaDeviceInfo *deviceInfo;
  for( int i=0; i<numDevices; i++ ) {
    deviceInfo = Pa_GetDeviceInfo( i ) ;
    std::cout << "\n" ;
    std::cout << "device number:\t"  << i << "\n" ;
    std::cout << "name:\t"  << std::string(deviceInfo->name) << "\n" ;

    std::cout << "hostApi:\t"  << deviceInfo->hostApi << " (" << std::string( Pa_GetHostApiInfo( deviceInfo->hostApi )->name ) << ")" << "\n" ;

    std::cout << "maxInputChannels:\t"  << deviceInfo->maxInputChannels << "\n" ;
    std::cout << "maxOutputChannels:\t"  << deviceInfo->maxOutputChannels << "\n" ;
    std::cout << "defaultLowInputLatency:\t"  << deviceInfo->defaultLowInputLatency << "\n" ;
    std::cout << "defaultLowOutputLatency:\t"  << deviceInfo->defaultLowOutputLatency << "\n" ;
    std::cout << "defaultHighInputLatency:\t"  << deviceInfo->defaultHighInputLatency << "\n" ;
    std::cout << "defaultHighOutputLatency:\t"  << deviceInfo->defaultHighOutputLatency << "\n" ;
    std::cout << "defaultSampleRate:\t"  << deviceInfo->defaultSampleRate << "\n" ;
    std::cout << std::flush ;
  }
  return 0;
}

PaDeviceIndex audio::search_device(const char* str){
  int numDevices = Pa_GetDeviceCount();
  const   PaDeviceInfo *deviceInfo;
  for( int i=0; i<numDevices; i++ ) {
    deviceInfo = Pa_GetDeviceInfo( i ) ;
    if ( string_search( deviceInfo->name, str) )
      return i;
  }  
  return paNoDevice;
};


void audio::start( ){
  start_callback_thread();
  start_stream() ;
}

///  Opens the audio device and starts the streamming.
///  A string can be given to specify a device. A search for the string is performed and,
///  if the real device name contains the string, the device is selected.
///  In no string is specified, then opens the default device.
int audio::open_device( const char* devname ){
  PaDeviceIndex d;
  if ( devname == 0 ){
    d = Pa_GetDefaultInputDevice(); // default input device
    if ( d == paNoDevice ) {
      std::cout << "Error: No default input device.\n" << std::endl ;
      return -1;
    }
  } else {
    d = search_device( devname ) ;
    if ( d == paNoDevice ){
      std::cout << "Error: cannot find device." << std::endl ;
      return -1;
    }
    std::cout << "\nFound device " << d << std::endl;
  }

  return open_device( d ) ;
};

int audio::open_device( PaDeviceIndex d ){

  PaStreamParameters inputParameters;

  //inputParameters.device = search_device("hw:0,0");
  //inputParameters.device = search_device("STM32");
  inputParameters.device = d ;

  /*
  std::cout << "\nFound device " << inputParameters.device << std::endl;
  if ( inputParameters.device == paNoDevice ){
    std::cout << "Error: cannot find device." << std::endl;
    return;
  }
  */
  /*
  inputParameters.device = Pa_GetDefaultInputDevice(); // default input device
  if (inputParameters.device == paNoDevice) {
    std::cout << "Error: No default input device.\n" << std::endl ;
    return;
  }
  */

  const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo( inputParameters.device );
  num_channels = deviceInfo->maxInputChannels;
  sample_rate =  deviceInfo->defaultSampleRate;

  inputParameters.channelCount = num_channels;
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;


  PaError err = Pa_OpenStream(
               &pa_stream,
               &inputParameters,
               NULL,                  /* &outputParameters, */
               sample_rate,
               FRAMES_PER_BUFFER,
               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
               external_callback,
               this );
  if( err != paNoError ){
    std::cout << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
    return -1;
  }

  pa_open = true;

  return 0;
}


void audio::start_stream(){
  PaError err = Pa_StartStream( pa_stream );
  if( err != paNoError ){
    std::cout << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
    return ;
  }

  pa_streamming = true;

  for (int c=0;c<num_channels;c++)
    levels[c] = 0.0f ;

}




int audio::callback( const void *inputBuffer, void *outputBuffer,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags )
{
   _framesPerBuffer = framesPerBuffer;
  
  const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
  //  SAMPLE *rptr2 = (SAMPLE*)inputBuffer;

  /* decay levels */
  for (int c=0; c<num_channels; c++ ) {
    levels[c] *= 0.9f ;
  }

  /*
  for(int i=0; i<framesPerBuffer; i++ ) {
    for (int c=0; c<num_channels; c++ ) {
      if (c==0){
	*rptr2 = sin( 2 * PI * (double)i/double(framesPerBuffer)    ) ;
      }
      rptr2++ ;
    }
  }
  */

  input_ring_buf.put( (float*) inputBuffer, framesPerBuffer * num_channels );;
  //_framesPerBuffer = framesPerBuffer * num_channels ;  
  for(int i=0; i<framesPerBuffer; i++ ) {
    for (int c=0; c<num_channels; c++ ) {
      float sample = *rptr++;
      float sample_abs = (sample<0) ? -sample : sample ;
      if ( levels[c] < sample_abs )
	levels[c] = sample_abs ;
    }
  }
  count++ ;
  return paContinue ;
}



static int external_callback( const void *inputBuffer, void *outputBuffer,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
  reinterpret_cast<audio*>(userData)->callback( inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags ) ;
}



float audio::get_level(int c){
  if (!levels) return 0.0f ;
  if ( (c < 0) | (c >= num_channels) ) return 0.0f ;
  return levels[c];
};



double audio::get_cpu_load(){
  if ( pa_stream == 0 )
    return 0.0;
  return Pa_GetStreamCpuLoad( pa_stream ) ;
};


int audio::get_data(float* d, int sz){
  return input_ring_buf.get(d,sz);
};



/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
////////////////////// callback_thread //////////////////////////////////
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

void audio::set_audio_proc(audio_proc* audiop, int buf_length ) {
  ap = audiop ;
  samples_per_call = buf_length ;
} ;


void* audio::callback_thread_helper(void * context){
  return ((audio *)context)->callback_thread();
};

void* audio::callback_thread() {
  if (ap) {
    ap->set_sample_rate( sample_rate ) ;
    ap->set_channels( num_channels ) ;
    ap->pre_start();
  }

  float buf[ samples_per_call * num_channels ];

  while ( !stop_proc_thread ) {

    if ( ap ) {

      while ( input_ring_buf.available() >= ( samples_per_call * num_channels ) ) {
	input_ring_buf.get( buf, samples_per_call * num_channels ) ;
	ap->callback( buf, num_channels, samples_per_call ) ;
      }

    }

    usleep( callback_thread_sleep_time_us ) ;

  }


  if (ap)
    ap->post_stop();

  pthread_exit( NULL ) ;
  return 0;
};

void audio::start_callback_thread() {
  stop_proc_thread = false ;
  callback_thread_sleep_time_us = 1000 ;
  if ( ! pthread_create( &proc_thread_id, NULL, callback_thread_helper, (void*)this ) )
    thread_running = true ;
};


void audio::stop_callback_thread() {
  if (!thread_running)
    return ;
  stop_proc_thread = true ;
  void* status;
  pthread_join( proc_thread_id, &status ) ;
};


//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
////////////////////// callback_thread //////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
