#ifndef audio_proc_h
#define audio_proc_h


class audio_proc{
 public:
  virtual void callback( float* buf, int Nch, int Nsamples ) = 0;
};


#endif
