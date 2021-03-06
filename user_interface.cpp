
#include "user_interface.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#include "ascii_meters.h"
#include "nonblock_keyboard.h"

#define PI 3.14159265358979

user_interface::user_interface( process* pp ) :
  p( pp )
{

} ;

user_interface::~user_interface() {

} ;



int user_interface::go() {

  std::cout << "user_interface::go()" << std::endl ;

  int num_channels = p->get_num_channels();

  /*
  float levels_max[ num_channels ] ;
  int levels_counts[ num_channels ] ;
  for ( int i=0; i<num_channels; i++ ) {
    levels_max[i] = 0.0 ;
    levels_counts[i] = 0 ;
  }
  */

  std::cout << "vu meters (dBFS) \n" ;

  for (int i=1; i<=num_channels; i++)
    std::cout << i%10;
  for (int i=1; i<=num_channels; i++)
    std::cout << "  " << std::setw(3) << i << "   " ;

  std::cout << "   Spectrum                                                     " ;

  std::cout << "-180\u00B0     0\u00B0    180\u00B0" ;

  std::cout << std::endl;

  double gui_rate = 25.0 ; // Hz
  useconds_t sleep_time_us = 1.0 / gui_rate * 1000000.0 ;

  nonblock_keyboard k ;
  bool stop = false;

  while ( 1 ) {


    if ( p->is_finished() )
      break ;
    if ( stop )
      break ;


    while ( k.kbhit() ) {
      int c = k.getch(); // consume the character
      //std::cout << "  key = " << c << std::flush ;
      switch ( c ){
      case 27: // esc
	stop = true ;
	break;
      default:
	break;
      }
    }



    std::cout << "\r" ;


    // ------ vu-meters ------
    for (int c=0; c<num_channels; c++ ) {
      print_vertical_bar_dB( p->get_level(c) ) ;
    }
    for (int c=0; c<num_channels; c++ ) {
      //print_bar( levels[c] ) ;
      //print_bar( p->get_level(c), levels_max[c], levels_counts[c] ) ;
      print_db( p->get_level(c) ) ;
      //print_vertical_bar( p->get_level(c) ) ;
      //print_horizontal_bar( p->get_level(c) ) ;
      print_horizontal_bar( p->get_level(c), 3 ) ;
    }

    /*
    static float phase = 0;
    float val = 0.5 + 0.6 * sin(phase) ;
    phase += .05;
    print_horizontal_bar( val, 3 ) ;
    //print_vertical_bar( val ) ;
    //print_arrow( 360 * val ) ;
    print_needle( val, 20 ) ;
    */

    processed_data* results = NULL ;
    processed_data* tmp_results ;
    while ( tmp_results = p->get_result() ) {
      delete results;
      results = tmp_results ;
    }
    if ( results ) {
      for (int i=0; i<results->Nspec; i++){
	print_vertical_bar_dB( results->spec[i] ) ;
      }
      print_needle( results->angle / (2*PI) + 0.5 , 20 ) ;
      std::cout << std::setw(6) << results->angle * 180.0/PI << "\u00B0" ;
    }
    delete results ;


    std::cout << std::flush ;

    usleep( sleep_time_us ) ;

  }

  std::cout << std::endl ;



} ;
