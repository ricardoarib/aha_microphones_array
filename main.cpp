

#include "portaudio.h"
#include <iostream>



int main(int argc, char** argv){
  PaError err;
  err = Pa_Initialize();


  if( err != paNoError ) {
    //printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    //return 1;
    goto error;
  };


  int numDevices;
  numDevices = Pa_GetDeviceCount();
  if( numDevices < 0 )  {
    std::cout << "ERROR: Pa_CountDevices returned "<< numDevices << std::endl;
    err = numDevices;
    goto error;
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



  err = Pa_Terminate();
  if( err != paNoError ) {
    goto error;
    //    printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
    //return 1;
  };


  return 0;


 error:
  std::cout << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl ;
  return 1;

}

