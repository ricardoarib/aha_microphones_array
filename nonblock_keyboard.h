#ifndef nonblock_keyboard_h
#define nonblock_keyboard_h

#include <termios.h>

class nonblock_keyboard
{
 public:
  nonblock_keyboard() ;
  ~nonblock_keyboard() ;
  void reset_terminal_mode() ;
  void set_conio_terminal_mode() ;
  int kbhit() ;
  int getch() ;

 private:
  struct termios orig_termios;
};

#endif
