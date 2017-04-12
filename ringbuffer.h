#ifndef ringbuffer_h
#define ringbuffer_h

#include "pa_ringbuffer.h"

template<typename T>
class ringbuffer{
 public:
  ringbuffer(int size);
  ~ringbuffer();
  int put(T* d, int sz);
  int get(T* d, int sz);
  int available(); // to read
  int free_space(); // to write

 private:
  T* data;
  PaUtilRingBuffer ringBuffer;
  int sz_samples;
  int numBytes;
};


#endif
