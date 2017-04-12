
#include "user_interface.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <cmath>
#include <sndfile.h>
#include <unistd.h>
#include <getopt.h>

#include "ascii_meters.h"

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
  std::cout << std::endl;



  float total_rec_time = 3; // seconds
  double gui_rate = 25.0 ; // Hz
  useconds_t sleep_time_us = 1.0 / gui_rate * 1000000.0 ;


  while ( 1 ) {
    if ( p->is_finished() )
      break ;
    std::cout << "\r" ;

    /*
    char kp ;
    std::cin >> kp ;
    if ( kp ) {
      std::cout << "Pressed key: " << kp << std::endl ;
      break ;
    }
    */


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


    static float phase = 0;
    float val = 0.5 + 0.6 * sin(phase) ;
    phase += .05;
    print_horizontal_bar( val, 3 ) ;
    //print_vertical_bar( val ) ;
    //print_arrow( 360 * val ) ;
    print_needle( val, 20 ) ;


    std::cout << std::flush ;

    usleep( sleep_time_us ) ;

  }

  std::cout << std::endl ;



} ;
