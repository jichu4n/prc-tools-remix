/* C support library random number functions for Palm OS.

   Author: John Marshall  (They're not very complicated :-))

   This code is in the public domain.  */

#include "stdlib.h"

#include <System/SysUtils.h>
#include <System/TimeMgr.h>

#ifdef Lrand

int
rand (void) {
  return SysRandom (0);
  }

#endif
#ifdef Lsrand

void
srand (unsigned int seed) {
  SysRandom ((seed)? seed : TimGetTicks ());
  }

#endif
