#ifndef ascii_meters_h
#define ascii_meters_h

float val_to_dB_fraction( float val ) ;

void print_db( float val ) ;
void print_bar(float val, float& max, int& cnt) ;
void print_vertical_bar( float val ) ;
void print_vertical_bar_dB( float val ) ;
void print_horizontal_bar( float val, int wide = 1 ) ;
void print_horizontal_bar_dB( float val, int wide = 1 ) ;

void print_arrow( float val ) ;
void print_needle( float val, int N ) ;

#endif
