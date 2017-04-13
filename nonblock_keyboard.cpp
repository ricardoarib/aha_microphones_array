/*
Based on:
    http://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input#448982
    https://gist.github.com/whyrusleeping/3983293
*/




#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <iostream>

#include "nonblock_keyboard.h"

//struct termios orig_termios;

void nonblock_keyboard::reset_terminal_mode()
{
  tcsetattr(0, TCSANOW, &orig_termios);
}


nonblock_keyboard::nonblock_keyboard() {
  set_conio_terminal_mode();
  return ;

  
  while (1) {
    while (!kbhit()) {
      /* do some work */
    }
    //(void)getch(); /* consume the character */
    int c = getch(); /* consume the character */
    std::cout << "c = " << c << std::endl ;
    if ( c==3 | c==27)
      break ;
  }
}

nonblock_keyboard::~nonblock_keyboard() {
  reset_terminal_mode();
}

void nonblock_keyboard::set_conio_terminal_mode()
{
  struct termios new_termios;

  /* take two copies - one for now, one for later */
  tcgetattr(0, &orig_termios);
  memcpy(&new_termios, &orig_termios, sizeof(new_termios));

  /* register cleanup handler, and set the new terminal mode */
  //atexit(reset_terminal_mode); // No need for this. reset_terminal_mode is called in the destructor.
  //cfmakeraw(&new_termios);

  new_termios.c_lflag &= ~ICANON;
  new_termios.c_lflag &= ~ECHO;
  //new_termios.c_lflag &= ~ISIG;
  new_termios.c_cc[VMIN] = 0;
  new_termios.c_cc[VTIME] = 0;
  tcsetattr(0, TCSANOW, &new_termios);
}

int nonblock_keyboard::kbhit()
{
  struct timeval tv = { 0L, 0L };
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(0, &fds);
  return select(1, &fds, NULL, NULL, &tv);
}

int nonblock_keyboard::getch()
{
  int r;
  unsigned char c;
  if ((r = read(0, &c, sizeof(c))) < 0) {
    return r;
  } else {
    return c;
  }
}

