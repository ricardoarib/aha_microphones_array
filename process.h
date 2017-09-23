#ifndef process_h
#define process_h

#include "audio_proc.h"
#include "ringbuffer.h"

#include <sndfile.h>
#include <string>
#include <pthread.h>


struct processed_data {
  float angle_geo, angle_srp ;
  float spec[256] ;
  int Nspec ;
  processed_data(){
    angle_geo = 0 ;
    angle_srp = 0;
    Nspec = 0 ;
  };
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
    void set_room_dimensions(float length, float width, float cell_size) ;
    void set_mics_centroid_position( float x, float y ) ; // (x,y) in meters
    
    int Nsamples;
    
    int write_file_variable; // 
    
    int set_nsamples (int Nsamples);
    float findMaxVal_2D (float ** matrix, int dim1, int dim2);


  // optional
  float get_level( int c ) ;
  int get_num_channels() { return channels; } ;
  bool is_finished() { return processing_finished ;  } ;
  void set_samples_to_process( int count ) ;
  processed_data* get_result() ; // Do not forget to delete the data pointed to by the address returned after done using it.
    // Centroid coordinates
    float centroid_x, centroid_y;
    
    
    
 private: // !!!
  void open_snd_file() ;
  void close_snd_file() ;
  void send_results( processed_data* d ) ;
    void fill_grid();
    void fill_zeros_2d_grid (float ** grid, int dim1, int dim2);
    void fill_grid2(int Nfft, int *** grid2);
    void createEnergyMap ( int Nfft, int *** grid, float ** correl, float ** energy_map);
    void setToZeroImpossibleLocations (float ** energy_map);
    float getAngle ( float x, float y);
    
    void fft_write (int Nfft, float * signal, float * result_fft_RE, float * result_fft_IM);


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
    
#define ROOM_LENGTH 1
#define ROOM_WIDTH 1
#define CELL_SIZE 0.1
#define NUM_MIC_PAIRS 4
#define SOUND_SPEED 343.21
    
    int num_mic_pairs = 4;
    int room_width_n, room_length_n; // [ cells ]
    float cell_size; // [ m / cell ]
    
    // Criar GRID
    float grid [NUM_MIC_PAIRS][ (int) (ROOM_LENGTH / CELL_SIZE) ][ (int) (ROOM_WIDTH / CELL_SIZE) ];
    
    int *** grid2; // 3 dimentions
    float c = 343.21;
    
    float ** correl;
    float ** energy_map;
    
    
    // Pseudo inverse
    float pinv[2][8] = {2.500000000000000, 1.767766952966369, 0.000000000000000, -1.767766952966368, -2.500000000000000, -1.767766952966369, -0.000000000000000, 1.767766952966368, -0.000000000000001, 1.767766952966370, 2.500000000000000, 1.767766952966369, 0.000000000000001, -1.767766952966369, -2.500000000000000, -1.767766952966369};
    
    // Mics position
    float mics[8][2] = {
        {0.1, 0},
        {0.070710678118655, 0.070710678118655},
        {0, 0.1},
        {-0.070710678118655, 0.070710678118655},
        {-0.1, 0},
        {-0.070710678118655, -0.070710678118655},
        {0, -0.1},
        {0.070710678118655, -0.070710678118655}
    };
    
    
    // Mic pairs
    int pairs[4][2]= {{0,4}, {1,5}, {2,6}, {3,7}};

};


#endif
