#ifndef audio_proc_h
#define audio_proc_h


class audio_proc{
 public:
  virtual void callback( float* buf, int Nch, int Nsamples ) = 0;
  virtual void set_sample_rate( float sample_rate ) = 0;
  virtual void set_channels( int channels ) = 0;
  virtual void pre_start() = 0;
  virtual void post_stop() = 0;
};


#endif
