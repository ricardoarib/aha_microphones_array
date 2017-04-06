#ifndef ui_h
#define ui_h

#include "audio.h"
#include "process.h"


class user_interface
{
 public:
  user_interface( audio* a, process* p ) ;
  ~user_interface() ;
  int go() ;
 private:
  audio* a ;
  process* p ;
};

#endif
