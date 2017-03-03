#include "audio.h"

#include "portaudio.h"
#include <iostream>


#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_RATE  (16000)
#define FRAMES_PER_BUFFER (512)


static int external_callback( const void *inputBuffer, void *outputBuffer,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
			      void *userData ) ;


audio::audio() :
  count(0),
  pa_init(false), pa_open(false), pa_streamming(false),
  num_channels(0),
  levels(0)
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
  if (!pa_init) return;
  PaError err = Pa_Terminate();
  if( err != paNoError ) {
    std::cout << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;    //goto error;
    //printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    //return 1;
  };
  delete [] levels;
};



int audio::list_devices_info(){
  
  int numDevices;
  numDevices = Pa_GetDeviceCount();
  if( numDevices < 0 )  {
    std::cout << "ERROR: Pa_CountDevices returned "<< numDevices << std::endl;
    //err = numDevices;
    //goto error;
    return -1;
  }

  std::cout << "Pa_CountDevices returned "<< numDevices << std::endl;

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




void audio::open_device(){

  PaStreamParameters inputParameters;
  PaStream* stream;
  inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
  if (inputParameters.device == paNoDevice) {
    std::cout << "Error: No default input device.\n" << std::endl ;
    return;
  }
  num_channels = 10;
  inputParameters.channelCount = 10;
  inputParameters.sampleFormat = PA_SAMPLE_TYPE;
  inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo = NULL;


  PaError err = Pa_OpenStream(
               &stream,
               &inputParameters,
               NULL,                  /* &outputParameters, */
               SAMPLE_RATE,
               FRAMES_PER_BUFFER,
               paClipOff,      /* we won't output out of range samples so don't bother clipping them */
               external_callback,
               this );
  if( err != paNoError ){
    std::cout << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
    return ;
  }

  err = Pa_StartStream( stream );
  if( err != paNoError ){
    std::cout << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
    return ;
  }

  pa_open = true;
  for (int c=0;c<num_channels;c++)
    levels[c] = 0.0f ;

}





int audio::callback( const void *inputBuffer, void *outputBuffer,
			   unsigned long framesPerBuffer,
			   const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags)
{
  const SAMPLE *rptr = (const SAMPLE*)inputBuffer;

  /* decay levles */
  for (int c=0; c<num_channels; c++ ) {
    levels[c] *= 0.9f ;
  }

  for(int i=0; i<framesPerBuffer; i++ ) {
    for (int c=0; c<num_channels; c++ ) {
      float sample = *rptr++;
      float sample_abs = (sample<0) ? -sample : sample ;
      if ( levels[c] < sample_abs )
	levels[c] < sample_abs ;
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
  audio *a = reinterpret_cast<audio*>(userData);
  a->callback( inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags ) ;
}



float audio::get_level(int c){
  if (!levels) return 0.0f ;
  if (c < 0 | c <= num_channels) return 0.0f ;
  return levels[c];
};
