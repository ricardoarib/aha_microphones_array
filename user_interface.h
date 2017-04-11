#ifndef ui_h
#define ui_h

#include "process.h"


class user_interface
{
 public:
  user_interface( process* p ) ;
  ~user_interface() ;
  int go() ;
 private:
  process* p ;
};

#endif
