#ifndef process_h
#define process_h

#include "audio_proc.h"

#include <sndfile.h>

class process : public audio_proc
{
 public:
  process();
  ~process();

   void callback( float* buf, int Nch, int Nsamples ) ;
   void set_sample_rate( float val ) ;
   void set_channels( int val ) ;
   void pre_start() ;
   void post_stop() ;

 private:
  void open_snd_file() ;
  void close_snd_file() ;

  SNDFILE* infile ;
  double sample_rate ;
  int channels ;
};


#endif
