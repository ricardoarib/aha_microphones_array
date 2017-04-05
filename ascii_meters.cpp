#include "ascii_meters.h"
#include <cmath>
#include <iostream>
#include <iomanip>



#define MAX_DB 0.0f
#define MIN_DB -50.0f


void print_db( float val ) { /*!< Value*/
  float val_dB = 20*log10( val ) ;
  //std::cout.precision(0);
  std::cout << std::setw(5) << std::setprecision(1) ;
  //std::cout << std::fixed << " " << val_dB << "dBFS" ;
  std::cout << std::fixed << val_dB ;
}

void print_bar(float val, float& max, int& cnt){ /*!< Value*/
    #define START_CNT 25
  if ( val >= max ) {
    max = val ;
    cnt = START_CNT ;
  } else {
    cnt-- ;
  }

  if ( cnt <= 0 ){
    max = val ;
    cnt = START_CNT ;
  }

  int N =  8; // bar total length
  float max_dB = MAX_DB;
  float min_dB = MIN_DB;
  float val_dB = 20*log10(val) ;
  float val_max_dB = 20*log10(max) ;

  //int n = N+val_log*4*N ;
  int n     = ( val_dB     - min_dB)/(max_dB-min_dB) * N;
  int n_max = ( val_max_dB - min_dB)/(max_dB-min_dB) * N;

  if ( n > N )
    n = N;
  //if ( n < 0 )
  //  n = 0;

  if ( n_max > N )
    n_max = N;

  for (int i=0; i< N; i++) {
    if ( i == n_max )
      std::cout << '>';
    else if ( i <= n )
      std::cout << '#';
    else
      std::cout << ' ';
  }
};

void print_vertical_bar( float val ){ /*!< Value*/

  //wchar_t bars[] = " ▁▃▄▅▆▇█" ;
  char const *bars[] = { " ", "\u2581", "\u2582","\u2583", "\u2584", "\u2585", "\u2586", "\u2587", "\u2588" } ;

  int N =  9; // bar total length
  float max_dB = MAX_DB;
  float min_dB = MIN_DB;
  float val_dB = 20*log10(val) ;

  int n     = ( val_dB     - min_dB)/(max_dB-min_dB) * N;

  if ( n > N )
    n = N;
  if ( n < 0 )
    n = 0;

  std::cout << bars[n] ;
};


void print_horizontal_bar( float val, int wide ){ /*!< Value*/

  //wchar_t bars[] = "▏▎▍▌▋▊▉" ;
  char const *bars[] = { "\u258F", "\u258E","\u258D", "\u258C", "\u258B", "\u258A", "\u2589", "\u2588" } ;

  int N =  8; // bar total length
  float max_dB = MAX_DB;
  float min_dB = MIN_DB;
  float val_dB = 20.0f*log10(val) ;

  float nf    = ( val_dB     - min_dB)/(max_dB-min_dB) * (float)(N * wide) ;

  if ( nf > N*wide )
    nf = N*wide;
  //if ( nf < 0 )
  //  nf = 0;

  int n_big = nf / N ;
  int n_small =   nf - n_big * N ;

  //std::cout << "\r" << "n_big = " << n_big << "   n_small = " << n_small << "  ";

  for (int i=0; i<wide; i++){
    if ( i < n_big )
      std::cout << bars[ N-1 ] ;
    else if ( ( i == n_big ) && ( n_small >= 0 ) )
      std::cout << bars[ n_small ] ;
    else
      std::cout << ' ' ;
  }

  //std::cout << std::endl ;

};

