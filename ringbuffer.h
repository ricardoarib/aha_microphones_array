#ifndef ringbuffer_h
#define ringbuffer_h

#include "pa_ringbuffer.h"

class ringbuffer{
 public:
  ringbuffer(int size);
  ~ringbuffer();
  int put(float* d, int sz);
  int get(float* d, int sz);
  int available();

 private:
  float* data;
  PaUtilRingBuffer ringBuffer;
  int sz_samples;
  int numBytes;
};


#endif
