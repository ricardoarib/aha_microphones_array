#include "ascii_meters.h"
#include <cmath>
#include <iostream>
#include <iomanip>



#define MAX_DB 0.0f
#define MIN_DB -50.0f

float val_to_dB_fraction( float val ) {
  float max_dB = MAX_DB ;
  float min_dB = MIN_DB ;
  float val_dB = 20.0f * log10( val ) ;

  float nf    = ( val_dB - min_dB ) / ( max_dB - min_dB ) ;
  return nf;
}



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


void print_vertical_bar_dB( float val ){ /*!< Value*/
  float nf = val_to_dB_fraction( val ) ;
  print_vertical_bar( nf ) ;
}

void print_vertical_bar( float val ){ /*!< Value*/

  //wchar_t bars[] = " ▁▃▄▅▆▇█" ;
  char const *bars[] = { " ", "\u2581", "\u2582","\u2583", "\u2584", "\u2585", "\u2586", "\u2587", "\u2588" } ;

  int N =  9; // bar total length

  //float max_dB = MAX_DB;
  //float min_dB = MIN_DB;
  //float val_dB = 20*log10(val) ;

  //int n     = ( val_dB     - min_dB)/(max_dB-min_dB) * N;

  int n = val * N ;

  if ( n > N )
    n = N;
  if ( n < 0 )
    n = 0;

  std::cout << bars[n] ;
};


void print_horizontal_bar_dB( float val, int wide ){ /*!< Value*/
  float nf = val_to_dB_fraction( val ) ;
  print_horizontal_bar( nf, wide ) ;
}

void print_horizontal_bar( float val, int wide ){ /*!< Value*/

  //wchar_t bars[] = "▏▎▍▌▋▊▉" ;
  char const *bars[] = { "\u258F", "\u258E","\u258D", "\u258C", "\u258B", "\u258A", "\u2589", "\u2588" } ;

  int N =  8; // bar total length

  //float max_dB = MAX_DB;
  //float min_dB = MIN_DB;
  //float val_dB = 20.0f*log10(val) ;

  //float nf    = ( val_dB     - min_dB)/(max_dB-min_dB) * (float)(N * wide) ;

  float nf = val * (float)(N * wide) ;

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


void print_arrow( float val ){ /*!< Value*/

  //wchar_t bars[] = "▏▎▍▌▋▊▉" ;
  char const *bars[] = { "\u258F", "\u258E","\u258D", "\u258C", "\u258B", "\u258A", "\u2589", "\u2588" } ;

  //  "⭢⭧⭡⭦⭠⭩⭣⭨"   (Miscellaneous Symbols and Arrows)
  char const *arrows1[] = { "\u2B62", "\u2B67", "\u2B61", "\u2B66", "\u2B60", "\u2B69", "\u2B63", "\u2B68" } ;

  //  "→↗↑↖←↙↓↘"   (Arrows)
  char const *arrows2[] = { "\u2192", "\u2197", "\u2191", "\u2196", "\u2190", "\u2199", "\u2193", "\u2198" } ;

  //  "⇒⇗⇑⇖⇐⇙⇓⇘"   (Arrows)
  char const *arrows3[] = { "\u21D2", "\u21D7", "\u21D1", "\u21D6", "\u21D0", "\u21D9", "\u21D3", "\u21D8" } ;

  //  "⮕⬈⬆⬉⬅⬋⬇⬊"   (Miscellaneous Symbols and Arrows)
  char const *arrows4[] = { "\u2B95", "\u2B08", "\u2B06", "\u2B09", "\u2B05", "\u2B0B", "\u2B07", "\u2B0A" } ;


  int N =  8; // bar total length

  int n = val / 360 * (N+1) ;

  if ( n >= N )
    n = 0;
  if ( n < 0 )
    n = 0;

  std::cout << arrows4[ n ] ;

};

