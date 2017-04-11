#ifndef process_h
#define process_h

#include "audio_proc.h"

#include <sndfile.h>
#include <string>

class process : public audio_proc
{
 public:
  // mandatory
  process( std::string filename ) ;
  ~process() ;
   void callback( float* buf, int Nch, int Nsamples ) ;
   void set_sample_rate( float val ) ;
   void set_channels( int val ) ;
   void pre_start() ;
   void post_stop() ;


   // optional
   float get_level( int c ) ;
   int get_num_channels() { return channels; } ;
 private:
  void open_snd_file() ;
  void close_snd_file() ;


  SNDFILE* infile ;
  double sample_rate ;
  int channels ;
  std::string outfilename ;

  int count ;
  float levels[256] ;

};


#endif
