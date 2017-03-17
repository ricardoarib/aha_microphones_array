
#include "ringbuffer.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include <iostream> // debug only

ringbuffer::ringbuffer(int size) :
  sz_samples(size)
{
  
  numBytes = size * sizeof(float);
  data = (float*) malloc(numBytes);
  // TODO: check for malloc errors
  if ( PaUtil_InitializeRingBuffer( &ringBuffer, sizeof(float), size, data ) < 0 ) {
    std::cout << "Failed to initialize ring buffer. Size is not power of 2 ?? size = " << size << std::endl;
  }
  
};


ringbuffer::~ringbuffer(){
  free( data );
};


int ringbuffer::put(float* d, int sz){
  ring_buffer_size_t elementsFree = PaUtil_GetRingBufferWriteAvailable( &ringBuffer ) ;
  int sz_to_put = ( elementsFree < sz )? elementsFree : sz ;
  if ( !sz_to_put ) {
    return 0;
  }
  if ( sz_to_put != sz ) {
    return 0;
  }
  void *ptr[2];
  ring_buffer_size_t sizes[2];
  int idx[2] = { 0, 0 } ;
  PaUtil_GetRingBufferWriteRegions( &ringBuffer, sz_to_put, ptr + 0, sizes + 0, ptr + 1, sizes + 1 ) ;
  idx[1] = sizes[0] ;

  for ( int i = 0; i < 2 && ptr[i] != NULL; ++i ) {
    memcpy( ptr[i], d+idx[i], sizes[i]*sizeof(float) );
  }
  PaUtil_AdvanceRingBufferWriteIndex( &ringBuffer, sz_to_put ) ;
  
  return sz_to_put ;
  
};


int ringbuffer::get(float* d, int sz){
  ring_buffer_size_t elementsInBuffer = PaUtil_GetRingBufferReadAvailable( &ringBuffer ) ;
  int sz_to_get = ( elementsInBuffer < sz )? elementsInBuffer : sz ;
  if ( !sz_to_get ) {
    return 0;
  }
  if ( sz_to_get != sz ) {
    return 0;
  }
  void *ptr[2];
  ring_buffer_size_t sizes[2];
  int idx[2] = { 0, 0 } ;
  ring_buffer_size_t elementsRead = PaUtil_GetRingBufferReadRegions( &ringBuffer, sz_to_get, ptr + 0, sizes + 0, ptr + 1, sizes + 1 ) ;
  idx[1] = sizes[0] ;

  if ( elementsRead )  {
    for ( int i = 0; i < 2 && ptr[i] != NULL; ++i ) {
      memcpy( d+idx[i], ptr[i], sizes[i]*sizeof(float) );
    }
    PaUtil_AdvanceRingBufferReadIndex( &ringBuffer, elementsRead ) ;
  }
  
  return elementsRead ;
  
};



