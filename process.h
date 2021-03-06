#ifndef process_h
#define process_h

#include "audio_proc.h"
#include "ringbuffer.h"

#include <sndfile.h>
#include <string>
#include <pthread.h>


struct processed_data {
  float angle ;
  float spec[256] ;
  int Nspec;
};


class process : public audio_proc
{
 public:
  // mandatory
  process( std::string filename, int count = -1 ) ;
  ~process() ;
  void callback( float* buf, int Nch, int Nsamples ) ;
  void set_sample_rate( float val ) ;
  void set_channels( int val ) ;
  void pre_start() ;
  void post_stop() ;


  // optional
  float get_level( int c ) ;
  int get_num_channels() { return channels; } ;
  bool is_finished() { return processing_finished ;  } ;
  void set_samples_to_process( int count ) ;
  processed_data* get_result() ; // Do not forget to delete the data pointed to by the address returned after done using it.
 private:
  void open_snd_file() ;
  void close_snd_file() ;
  void send_results( processed_data* d ) ;


  SNDFILE* infile ;
  double sample_rate ;
  int channels ;
  std::string outfilename ;

  int count ;
  float levels[256] ;
  pthread_mutex_t mutex_levels;

  int sample_count ;
  int sample_limit_number ;
  bool limit_sample_number ;
  bool processing_finished ;

  ringbuffer<void*> rb_results ;
};


#endif
