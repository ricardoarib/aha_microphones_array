#ifndef process_h
#define process_h

#include "audio_proc.h"

class process : public audio_proc
{
 public:
  virtual void callback( float* buf, int Nch, int Nsamples ) ;
};


#endif
