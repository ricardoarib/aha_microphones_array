
#include "process.h"
#include <iostream>
#include <cmath>
#include "kiss_fft.h"
#include <math.h>
#include <iomanip>
#include <fstream>


using namespace std;

#define RESULTS_RING_BUFFER_SIZE 1024*sizeof(void*)
#define PI 3.14159265358979
#define FS 48000
#define max_mics_distance 0.2

/*
#define ROOM_LENGTH 1
#define ROOM_WIDTH 1
#define CELL_SIZE 0.1
#define NUM_MIC_PAIRS 4
*/

process::process( std::string fn, int count ) :
  sample_rate( 1 ),
  channels( 1 ),
  outfilename( fn ),
  count( 0 ),
  //levels( 0 )
  sample_count( 0 ),
  sample_limit_number( 0 ),
  limit_sample_number( false ),
  processing_finished( false ),
  rb_results( RESULTS_RING_BUFFER_SIZE )
{
  set_samples_to_process( count ) ;

  pthread_mutex_init( &mutex_levels, NULL ) ;

  pthread_mutex_lock( &mutex_levels ) ;
  //levels = new float[256] ;
  for (int i=0; i< 256; i++)
    levels[i] = 0 ;
  pthread_mutex_unlock( &mutex_levels ) ;
    
    num_mic_pairs = NUM_MIC_PAIRS;
    cell_size = CELL_SIZE;
    room_length_n = (int)(ROOM_LENGTH/cell_size);
    room_width_n = (int)(ROOM_WIDTH/cell_size);
    
    set_room_dimensions(2.0, 2.0, 0.2);
};


process::~process(){
  //delete [] levels ;
};

// ************************************************************************************************************************
// ***



//Erasable..........................................................................
void createSinusoidSignal (int Npoints, float * buffer){
    float rate, result;
    
    for (int i = 0; i<Npoints; i++){
        rate = 10.0 * i;
        result = cos (rate * PI/180);
        buffer [i] = result;
    }
}
//..................................................................................


//Erasable..........................................................................
void addDelayToSignal (int Npts, float * signal, float * delayed_signal, float delay, int fs){
    int length = Npts;
    int delay_samples = (int) delay/fs;
    
    if (delay > 0){
        for (int i = 0; i<delay_samples; i++){
            delayed_signal[i] = 0;
        }
        int a = 0;
        for (int i = delay_samples; i<length; i++){
            delayed_signal[i] = signal[a];
            a++;
        }
    }else if(delay < 0){
        // to do, if needed
    }else return ;
}
//.................................................................................


float abs (float value){
    value = sqrt(value * value);
    return value;
}

int findMaxIdx (float * signal, int signal_length){
    
    int margin = 5;
    int range = max_mics_distance/340*FS + margin;
    
    //std::cout << "Range: "<<range<< std::endl;
    
    int max_idx = 0;
    
    for (int i = 0; i < range; i++){
        
        if (signal[max_idx] < signal[i]){
            max_idx = i;
        }
        
    }
    for (int i = signal_length-range; i < signal_length; i++){
        
        if (signal[max_idx] < signal[i]){
            max_idx = i;
        }
        
    }
    return max_idx;
}

float finMaxVal (float * signal, int signal_length){
    
    int max_idx;
    float max_val;
    
    max_idx = findMaxIdx (&signal[0], signal_length);
    max_val = signal[max_idx];
    
    return max_val;

}


void fft (int Nfft, float * signal, float * result_fft_RE, float * result_fft_IM){
 
    kiss_fft_cfg cfg = kiss_fft_alloc ( Nfft, 0, 0, 0);
    kiss_fft_cpx cx_in [Nfft];
    kiss_fft_cpx cx_out [Nfft];
 
    for (int i = 0; i<Nfft; i++){
        cx_in[i].r = signal[i];
        cx_in[i].i = 0.0;
    }
    kiss_fft (cfg , cx_in, cx_out);
 
    for (int i = 0; i<Nfft; i++){
        result_fft_RE [i] = cx_out[i].r;
        result_fft_RE [i]  *= 1.0 / (float)Nfft ;
     
        result_fft_IM [i] = cx_out[i].i;
        result_fft_IM [i] *= 1.0 / float(Nfft);
    }
    free(cfg);

}

void xcCalculation (int Nfft, float * spec1_RE, float * spec1_IM, float * spec2_RE, float * spec2_IM, int fs, float* xc){
    kiss_fft_cfg i_cfg = kiss_fft_alloc( Nfft, 1, NULL, NULL);
    kiss_fft_cpx i_cx_in [Nfft];
    kiss_fft_cpx i_cx_out [Nfft];
    
    
    for (int i = 0; i<Nfft; i++){
        i_cx_in[i].r = (spec1_RE[i] * spec2_RE[i] + spec1_IM[i] * spec2_IM[i])/(sqrt( spec1_RE[i] * spec1_RE[i] + spec1_IM[i] * spec1_IM[i] ) * sqrt( spec2_RE[i] * spec2_RE[i] + spec2_IM[i] * spec2_IM[i] ));
        i_cx_in[i].i = (spec1_IM[i] * spec2_RE[i] - spec1_RE[i] * spec2_IM[i])/(sqrt( spec1_RE[i] * spec1_RE[i] + spec1_IM[i] * spec1_IM[i] ) * sqrt( spec2_RE[i] * spec2_RE[i] + spec2_IM[i] * spec2_IM[i] ));
    }
    
    kiss_fft (i_cfg , i_cx_in, i_cx_out);
    
    for (int i = 0; i<Nfft; i++){
        //xc[i] = sqrt( i_cx_out[i].r * i_cx_out[i].r + i_cx_out[i].i * i_cx_out[i].i ); //abs
        xc[i] = i_cx_out[i].r; //real
        xc[i]  *= 1.0 / (float)Nfft ;
    }
    free(i_cfg);
    
}

int findDelay(int Nfft, float* xc){
    int max_idx = findMaxIdx(&xc[0], Nfft);
    if (max_idx > Nfft/2) max_idx -= Nfft;
    return max_idx;
}

int xcorr (int Nfft, float * spec1_RE, float * spec1_IM, float * spec2_RE, float * spec2_IM, int fs){

    float xc[Nfft];
    xcCalculation ( Nfft, spec1_RE, spec1_IM, spec2_RE, spec2_IM, fs, xc) ;

    int delay_of_max = findDelay( Nfft, xc) ;

    return delay_of_max;
    
}

int delayCalculation (int Nfft, int fs, float * signal1, float * signal2){
//  (!) Returns maximum index (not time delay!)

    //Signal 1
    float result_fft_RE[Nfft], result_fft_IM[Nfft];
    fft(Nfft, &signal1[0], &result_fft_RE[0], &result_fft_IM[0]);
    
    //Signal 2
    float result_fft2_RE[Nfft], result_fft2_IM[Nfft];
    fft(Nfft, &signal2[0], &result_fft2_RE[0], &result_fft2_IM[0]);
    
    //Generalized cross-correlation
    int max_idx = 0;
    max_idx = xcorr(Nfft, &result_fft_RE[0], &result_fft_IM[0], &result_fft2_RE[0], &result_fft2_IM[0], fs);
    
    return max_idx;
    
}

// Mics position

/*
 function [ pos ] = calcPos( angle, dist )
 % Converts polar coordinates into cartesian
 
 pos = dist * [ cos( angle * pi / 180)  sin( angle * pi /180 )];
 
 end
 */

void calcPos ( int angle, int dist, float * pos){
    pos[0] = dist * cos ( angle * PI /180 );
    pos[1] = dist * sin ( angle * PI /180 );
}


// SRP TOOLS

//----------- 1. Pair Microphones

/*
 num_mics = size(mics_position,1);
 num_pairs = floor(num_mics/2);
 dist_mtx = zeros (num_pairs, num_mics);
 taken = zeros (num_mics,1);
 pairs = zeros (num_pairs, 2);
 
 for i = 1:num_pairs
 taken (i) = 1;
 for j = 1:num_mics
 dist_mtx(i,j) = norm(mics_position(i,:)-mics_position(j,:));
 end
 [~, I] = max(dist_mtx(i,:));
 while ismember(0,taken)
 if taken(I) == 0
 pairs(i,1) = i;
 pairs(i,2) = I;
 taken(I) = 1;
 break;
 else
 dist_mtx(i,I)=0;
 [~, I] = max(dist_mtx(i,:));
 end
 end
 end
 
 pairs = pairs(all(pairs,2),:);
 */

/*

void pairMicrophones ( float * mic_pos, int num_mics, int * pair ){ // TODO
    int num_pairs = (int) num_mics / 2;
    
    float dist_mtx[num_pairs][num_mics] = 0;
}
 
 */

//----------- 2. Create Grid
/*
 function [ GRID ] = createGrid( room_width, room_length, cell_size, pairs, mics_position, fs )
 %CreateGrid Divides the room in cells and estimates the theoretical delay
 %between pairs of microphones (in samples)
 %   The dimensions of the room and the size of the cell are provided so the
 %   program goes through all of them and stores the theoretical TDOA (in
 %   samples) for each pair of microphones
 
 w = -room_width/2:cell_size:room_width/2; % rows = yy
 l = -room_length/2:cell_size:room_length/2; % columns = xx
 w = w* -1;
 num_pairs = size(pairs, 1);
 GRID = zeros ( length(w), length(l), num_pairs );
 t = zeros(1,2);
 c = 343.21; % Sound velocity in 20C air [ m/ s ]
 
 for i = 1:length(w) % each line
 for j = 1:length(l) % each column
 source_position = [l(j) w(i)];
 for k = 1:num_pairs
 t(1) = norm(source_position-mics_position(pairs(k,1),:))/c;
 t(2) = norm(source_position-mics_position(pairs(k,2),:))/c;
 D = t(1) - t(2);
 GRID(i,j,k) = round( D * fs );
 end
 end
 end
 
 end
 */

void createGrid (int room_length, int room_width, float cell_size, int * centroid_position_ROOM_FRAME, float * mics_position_CENTR_FRAME, int fs){
    int a;
}

//----------- 3. Fill Grid
/*
 function [ xc ] = computeCorrelations( delayed_signals, pairs )
 %UNTITLED8 Summary of this function goes here
 %   Detailed explanation goes here
 
 num_pairs = size(pairs,1);
 xc = zeros(length(delayed_signals(1,:))*2-1,num_pairs);
 
 for i = 1:num_pairs
 [xc(:,i), ~] = xcorr(delayed_signals(pairs(i,1),:), delayed_signals(pairs(i,2),:));
 end
 
 end
 

 */


//----------- 4. Build Energy Map


//----------- 5. Find maximum


//----------- 6. Export result


// ***
// ************************************************************************************************************************


void process::callback( float* buf, int Nch, int Nsamples ) {

  pthread_mutex_lock( &mutex_levels ) ;
  // decay levels
  for (int c=0; c<channels; c++ ) {
    levels[c] *= 0.1f ;
  }
  // update levels
  float* rptr = buf ;
  for(int i=0; i<Nsamples; i++ ) {
    for (int c=0; c<Nch; c++ ) {
      float sample = *rptr++ ;
      float sample_abs = (sample<0) ? -sample : sample ;
      if ( levels[c] < sample_abs )
	levels[c] = sample_abs ;
    }
  }
  pthread_mutex_unlock( &mutex_levels ) ;


  // ------ Has processing already finished? ------
  if ( processing_finished )
    return ;


  // ------ write to wav file ------
  sf_write_float( infile, buf, Nsamples * Nch ) ;


  // ------ allocate place to put the results ------
  processed_data* results = new processed_data ;


// ************************************************************************************************************************
// ***
    
    std::cout << std::setprecision(15) << std::fixed;
    
    // Store signals
    int number_mics = 8;
    float signals [number_mics][Nsamples];
    int Nfft = Nsamples;
    
    for (int channel = 0; channel < number_mics; channel++){
        //std::cout << "Signal #"<<channel<< std::endl;
        for (int i = 0; i < Nsamples; i++){
            signals [channel][i] = buf [channel + i*Nch];
            //std::cout << signals[channel][i] << std::endl;
        }
    }

    // Delay calculation
    int ref_channel = 0;
    int delays [number_mics]; //indices!
    float med_delay = 0;
    
    for (int i = 0; i<number_mics; i++){
        delays [i] = delayCalculation (Nfft, FS, &signals[ref_channel][0], &signals[i][0]);
        //delays[i] /= FS; // Nao funciona porque?? FS muito grande? passar para double?
        med_delay += delays[i];
    }
    med_delay = med_delay / Nch;
    
    
    // Send spec information
    float result_fft_RE[Nfft], result_fft_IM[Nfft];
    fft(Nfft, &signals[ref_channel][0], &result_fft_RE[0], &result_fft_IM[0]);
    for (int i = 0; i< Nfft/16/8; i++){
        results->spec[i] = sqrt( result_fft_RE[i] * result_fft_RE[i] + result_fft_IM[i] * result_fft_IM[i] ) ;
        results->spec[i] *= 1.0 / (float)Nfft ;
    }
    results->Nspec = Nfft/16/8;
    
    // Change reference
    //std::cout <<"\nDelays normalizados[ " ;
    for (int i = 0; i<number_mics; i++){
        delays [i] = delays[i] - med_delay;
        //std::cout <<"Delays normalizados["<<i<<"]: "<< delays[i]<< std::endl;
        //std::cout << delays[i] << ", " ;
    }
    //std::cout << " ]" << std::endl;
    
    // Optimization problem
   // float pinv[2][8] = {2.500000000000000, 1.767766952966369, 0.000000000000000, -1.767766952966368, -2.500000000000000, -1.767766952966369, -0.000000000000000, 1.767766952966368, -0.000000000000001, 1.767766952966370, 2.500000000000000, 1.767766952966369, 0.000000000000001, -1.767766952966369, -2.500000000000000, -1.767766952966369};
    
    float v[2];
    
    //std::cout << std::setprecision(15) << std::fixed;
    
    for (int i = 0; i<number_mics; i++){
        v[0] += pinv[0][i] * (float)delays[i];
        v[1] += pinv[1][i] * (float)delays[i];
    }
    
    // Normalize resulting vector
    float norm_v = sqrt( v[0] * v[0] + v[1] * v[1] );
    if (norm_v == 0) norm_v = 0.000000000000000000001;
    v[0] /= norm_v;
    v[1] /= norm_v;
    
    //std::cout << "\n v = ( " << v[0] << ", " << v[1] << " )" << std::endl ;
    
    // Determine angle
    float val_angle = 0;
 //   val_angle = atan (v[1]/v[0]);
 //   if (acos(v[0]) < PI/2) val_angle = asin(v[1]); // the vector v is already normalized
 //   else val_angle = PI-asin(v[1]);
    
    if (v[0] >= 0){ // the vector v is already normalized
        val_angle = asin(v[1]);
    }else{
        if (v[1] >= 0) val_angle = PI - asin(v[1]);
        else val_angle = -PI - asin(v[1]);
    }
    //std::cout << "This is the angle =" <<val_angle<< std::endl;
    results->angle = val_angle ;
    
    /*
     
    float position[2];
    // void calcPos ( int angle, int dist, float * pos){
    calcPos( -45, 5, &position[0]);
    std::cout << "\n x = " << position[0]<< " and y = "<<position[1]<< std::endl ;
    
     */
    
// ***
// ************************************************************************************************************************

    /*
  //  /!\ For testing purposes only /!\  Create a sine wave/ramp to send to gui.
  static float phase = 0 ;
  //float val = 0.5 + 0.6 * sin( phase ) ;
  phase += .05 ;
  if ( phase > PI )
    phase -= 2*PI ;
  float val = phase ;
  //results->angle = val ;
    
    
  // ------ Do an FFT ------
  //int Nfft = 64*16 ;
  kiss_fft_cfg cfg = kiss_fft_alloc( Nfft, 0, 0, 0 );
  kiss_fft_cpx cx_in[Nfft];
  kiss_fft_cpx cx_out[Nfft];

  // put kth sample in cx_in[k].r and cx_in[k].i
  int channel = 0;
  for (int i = 0; i< Nfft; i++){
    cx_in[i].r = buf[ i * Nch + channel ] ;
    cx_in[i].i = 0.0 ;
  }
  kiss_fft( cfg , cx_in , cx_out );

  // transformed. DC is in cx_out[0].r and cx_out[0].i
  for (int i = 0; i< Nfft/16; i++){
    results->spec[i] = sqrt( cx_out[i].r * cx_out[i].r + cx_out[i].i * cx_out[i].i ) ; // abs(...)
    results->spec[i] *= 1.0 / (float)Nfft ;
  }
  results->Nspec = Nfft/16 ;

  free(cfg);
     */

     
  // ------ Send the results ------

  send_results( results ) ;


  // ------ finish processing? ------
  sample_count += Nsamples ;
  if ( limit_sample_number & ( sample_count > sample_limit_number ) ) {
    processing_finished = true ;
    return ;
  }
}; // --- Fim da callback


void process::set_sample_rate( float val ) { sample_rate = val ; } ;

void process::set_channels( int val ) { channels = val ; } ;


void process::pre_start() {
  std::cout << "process::pre_start()" << std::endl ;
    // inicializar GRID
    fill_grid();
    open_snd_file();
    grid2 = new float ** [num_mic_pairs];
    for (int m = 0; m < num_mic_pairs; m++){
        grid2[m] = new float * [room_length_n];
        for (int n = 0; n < room_length_n; n++){
            grid2[m][n] = new float [room_width_n];
        }
    }
} ;

void process::fill_grid (){
    for (int m = 0; m < NUM_MIC_PAIRS; m++){
        for (int a = 0; a < (int)(ROOM_LENGTH/CELL_SIZE); a++) {
            for (int b = 0; b < (int)(ROOM_WIDTH/CELL_SIZE); b++){
                grid [m][a][b] = 0;
            }
        }
    }
}

void process::fill_grid2(int Nfft) {
    for (int m = 0; m < num_mic_pairs; m++){
        for (int a = 0; a < room_length_n; a++) {
            for (int b = 0; b < room_width_n; b++){
                grid2 [m][a][b] = 0;
            }
        }
    }
}

void process::set_room_dimensions(float length, float width, float cell) {
    cell_size = cell;
    room_width_n = width / cell;
    room_length_n = length / cell;
}

void process::post_stop() {
  std::cout << "process::post_stop()" << std::endl ;
  close_snd_file();
    //delete [] grid2; // detele com parentesis rectos por se tratar de um array (e nao de uma variavel)
    for (int m = 0; m < num_mic_pairs; m++){
        for (int n = 0; n < room_length_n; n++){
            delete [] grid2[m][n];
        }
        delete [] grid2[m];
    }
    delete [] grid2;
} ;



void process::open_snd_file() {
  SF_INFO info;
  info.format=SF_FORMAT_WAV | SF_FORMAT_PCM_16 ;
  //info.format=SF_FORMAT_WAV | SF_FORMAT_PCM_24 ;
  //info.format=SF_FORMAT_WAV | SF_FORMAT_FLOAT ;
  info.channels = channels ;
  info.samplerate = sample_rate ;
  //const char* outfilename = "foo2.wav" ;

  //infile =  sf_open(outfilename, SFM_WRITE, &info ) ;
  infile =  sf_open( outfilename.c_str(), SFM_WRITE, &info ) ;
  if ( not infile ) {
    std::cout << "Cannot open sound file: " << outfilename << std::endl ;
  }
};


void process::close_snd_file() {
  if ( sf_close( infile ) ) {
    std::cout << "Error closing sound file." << std::endl ;
  };
} ;


float process::get_level( int c) {
  if (!levels) return 0.0f ;
  if ( (c < 0) | (c >= channels) ) return 0.0f ;
  pthread_mutex_lock( &mutex_levels ) ;
  float val = levels[c] ;
  pthread_mutex_unlock( &mutex_levels ) ;
  return val ;
};



void process::set_samples_to_process( int count ){
  if ( count <= 0 ) {
    limit_sample_number = false ;
  } else {
    sample_limit_number = count ;
    limit_sample_number = true ;
  }
}


void process::send_results( processed_data* d ){

  if ( rb_results.free_space() ) {
    rb_results.put( (void**)&d, 1 ) ;
  } else {
    std::cout << "rb_results full!!" << std::endl ;
    delete d;
  }

};



processed_data* process::get_result() {
  if ( rb_results.available()  ) {
    processed_data* r ;
    rb_results.get( (void**)&r, 1 ) ;
    return r;
  }
  return 0;
};


