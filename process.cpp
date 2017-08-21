
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
    
    set_room_dimensions(2.0, 2.0, 0.2); // Length, width, cell size
    set_mics_centroid_position(1.0, 1.0);
    Nsamples = set_nsamples (1024);
};


process::~process(){
  //delete [] levels ;
};

/* ****************************** Functions ************************************************************************************ */


float abs (float value){
    value = sqrt(value * value);
    return value;
}

int findMaxIdx (float * signal, int signal_length){
    
    // Guarantee the value is feasible
    int margin = 5;
    int range = max_mics_distance/340*FS + margin;
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

float finMaxVal (float * signal, int signal_length){ // Array
    
    int max_idx;
    float max_val;
    
    max_idx = findMaxIdx (&signal[0], signal_length);
    max_val = signal[max_idx];
    
    return max_val;

}

float findMaxVal_2D (float ** matrix, int dim1, int dim2){ // Matrix
    
    int max_idx;
    float max_val = 0;
    
    for (int i = 0; i < dim1; i++){
        max_idx = findMaxIdx (&matrix[i][0], dim2);
        if ( max_val < matrix[i][max_idx]){
            max_val = matrix[i][max_idx];
        }
//        std::cout << "max_val inside loop: "<< max_val << std::endl;

    }
    
    return max_val;
    
    //TODO: juntar condicao para que o valor do máximo escolhido seja o mais afastado
    
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

void obtainXc ( float * signal_1, float * signal_2, int Nfft, int fs, float * xc ){
    // Sig 1
    float result_1_fft_RE[Nfft];
    float result_1_fft_IM[Nfft];
    // Sig 2
    float result_2_fft_RE[Nfft];
    float result_2_fft_IM[Nfft];
    
    // FFT calculation
    fft ( Nfft, signal_1, &result_1_fft_RE[0], &result_1_fft_IM[0]);
    fft ( Nfft, signal_2, &result_2_fft_RE[0], &result_2_fft_IM[0]);
    
    // Cross-correlation calculation
    xcCalculation ( Nfft, result_1_fft_RE, result_1_fft_IM, result_2_fft_RE, result_2_fft_IM, fs, xc);
    
}

int findDelay(int Nfft, float* xc){
    int max_idx = findMaxIdx(&xc[0], Nfft);
    if (max_idx > Nfft/2) max_idx -= Nfft;
    return max_idx;
}

int calcMaxXcorr (int Nfft, float * spec1_RE, float * spec1_IM, float * spec2_RE, float * spec2_IM, int fs){

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
    max_idx = calcMaxXcorr(Nfft, &result_fft_RE[0], &result_fft_IM[0], &result_fft2_RE[0], &result_fft2_IM[0], fs);
    
    return max_idx;
    
}

void calcPos ( int angle, int dist, float * pos){
    pos[0] = dist * cos ( angle * PI /180 );
    pos[1] = dist * sin ( angle * PI /180 );
}

/* ----------------------------- Process functions ------------------------------------------------------------------------------*/

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
    
    
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+++++++++++++++++++
    
    // SRP APPROACH
    
    // Calculate and store correlation per mic pair
    int num_corr = num_mic_pairs;
    //float correlation[num_corr][Nfft];
    
    //pairs[4][2]
    
    float xc[Nfft];
    int mic_a, mic_b;
    
    for ( int i = 0; i < num_mic_pairs; i++){
        mic_a = pairs[i][0];
        mic_b = pairs[i][1];
        
        /*
        std::cout << "Entrou aqui!! i=" << i<< std::endl;
        std::cout << "mic a: "<< mic_a << std::endl;
        std::cout << "mic b: "<< mic_b << std::endl;
         */
        
        obtainXc ( &signals [mic_a][0], &signals [mic_b][0], Nfft, sample_rate, &xc[0] );
        
        *(correl[i]) = *xc;
        //correlation[i][0] = xc[0];
        /*
        std::cout << "* correlation[i] = " << * correlation[i]<< std::endl;
        std::cout << "correlation[0][0] = " << correlation[0][0]<< std::endl;
        std::cout << "correlation[1][1] = " << correlation[1][1]<< std::endl;
        std::cout << "correlation[2][0] = " << correlation[2][0]<< std::endl;
        std::cout << "correlation[3][1] = " << correlation[3][1]<< std::endl;
        std::cout << "correlation[4][1] = " << correlation[4][1]<< std::endl;
         */

    }
    
    createEnergyMap ( Nfft, grid2, correl, energy_map) ;
    
    float peak;
    peak = findMaxVal_2D (energy_map, room_length_n, room_width_n);

    
   // std::cout << "peak: "<<peak<< std::endl;
    
    
    //+++++++++++++++++++
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    

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

int process::set_nsamples ( int Nsamples) { return Nsamples ; } ;

void process::pre_start() {
  std::cout << "process::pre_start()" << std::endl ;
    // inicializar GRID
    //fill_grid();
    open_snd_file();
    
    // Initialize grid
    grid2 = new float ** [num_mic_pairs];
    for (int m = 0; m < num_mic_pairs; m++){
        grid2[m] = new float * [room_length_n];
        for (int n = 0; n < room_length_n; n++){
            grid2[m][n] = new float [room_width_n];
        }
    }
    
    correl = new float * [num_mic_pairs];
    for (int i = 0; i < num_mic_pairs; i++){
        correl[i] = new float [Nsamples];
    }
    
    energy_map = new float * [room_length_n];
    for (int i = 0; i < room_length_n; i++){
        energy_map[i] = new float [room_width_n];
    }
    
    
    // Fill grid
    fill_zeros_2d_grid( correl, num_mic_pairs, Nsamples );
    fill_zeros_2d_grid( energy_map, room_length_n, room_length_n );
    fill_grid2 ( Nsamples, grid2 );
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

void process::fill_zeros_2d_grid (float ** grid, int dim1, int dim2){
    for (int a = 0; a < dim1; a++) {
        for (int b = 0; b < dim2; b++){
            grid [a][b] = 0;
        }
    }
}

void process::fill_grid2 ( int Nfft, float *** grid2) {
    for (int m = 0; m < num_mic_pairs; m++){
        // Select mics
        int mic_a_idx = pairs[m][0];
        int mic_b_idx = pairs[m][1];
        
        // Get mics' positions
        float mic_a_x = mics[mic_a_idx][0] + centroid_x; // TODO: confirmar se centroid_x ou centroid_y /!\
        float mic_a_y = mics[mic_a_idx][1] + centroid_y;
        
            //std::cout << "mic a = (" <<mic_a_x<<", "<< mic_a_y <<")" << std::endl ;
        
        float mic_b_x = mics[mic_b_idx][0] + centroid_x;
        float mic_b_y = mics[mic_b_idx][1] + centroid_y;

            //std::cout << "mic b = (" <<mic_b_x<<", "<< mic_b_y <<")" << std::endl ;

        
        for (int a = 0; a < room_length_n; a++) {
            for (int b = 0; b < room_width_n; b++){
                // Determine distance to every point in the grid to the centroid
                float d_a = sqrt ( ( a * cell_size - mic_a_x ) * ( a * cell_size - mic_a_x ) + ( b * cell_size - mic_a_x ) * ( b * cell_size - mic_a_x ) ); // ERRO?! mic_a_x =/= mic_a_y
                float d_b = sqrt ( ( a * cell_size - mic_b_x ) * ( a * cell_size - mic_b_x ) + ( b * cell_size - mic_b_y ) * ( b * cell_size - mic_b_y ) );
                
                /*
                std::cout << "------------ for cell:" << ( room_length_n * a ) + b << std::endl ;
                std::cout << "d_a: "<< d_a << std::endl ;
                std::cout << "d_b: "<< d_b << std::endl ;
                 */

                
                // Calculate the time of arrival
                float ta_a = d_a / SOUND_SPEED;
                float ta_b = d_b / SOUND_SPEED;
                
                // Get the Time Difference Of Arrival  (TDOA)
                float tdoa_ab = ta_a - ta_b;
                
                // Fill the grid
                grid2 [m][a][b] = tdoa_ab * sample_rate;
                
                //std::cout << "grid2["<<m<<"]["<<a<<"]["<<b<<"]: "<< (int) grid2 [m][a][b] << std::endl ;
            }
        }
    }
}

void process::createEnergyMap ( int Nfft, float *** grid, float ** correl, float ** energy_map) {
    
    int idx;
    
    for (int a = 0; a < room_length_n; a++) {
        for (int b = 0; b < room_width_n; b++){
            for (int m = 0; m < num_mic_pairs; m++){
                // Get idx
                idx = grid[m][a][b];
                
                // Fill the grid
                energy_map [a][b] = energy_map [a][b] + correl [m][idx];
                
                std::cout << "energy_map ["<<a<<"]["<<b<<"]: " <<energy_map [a][b]<< std::endl;
            }
        }
    }
}

void process::set_room_dimensions(float length, float width, float cell) {
    cell_size = cell;
    room_width_n = width / cell;
    room_length_n = length / cell;
}

void process::set_mics_centroid_position (float x, float y){
    // (x,y) are in meters
    centroid_x = x;
    centroid_y = y;
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
    
    //delete correl
    for (int i = 0; i < num_mic_pairs; i++){
        delete [] correl[i];
    }
    delete [] correl;
    
    //delete energy_map
    for (int i = 0; i < room_length_n; i++){
        delete [] energy_map[i];
    }
    delete [] energy_map;
    
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
