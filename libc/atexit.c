/* C support library atexit() function for Palm OS.

   Author: John Marshall  (It's not very complicated :-))

   This code is in the public domain.  */

#include "stdlib.h"

#include <MemoryMgr.h>
#include "NewTypes.h"

/* This is a global variable.  So atexit functions are only called for
   launch codes with globals (surprise!).  This isn't too bad because
   the main use for atexit is to call destructors for function static
   objects (which are themselves globals!).  We could store the handle in
   a feature instead and avoid needing globals, but it doesn't seem to be
   worthwhile to communicate a suitable creator ID to these routines.  */

static MemHandle exitfuncsH = NULL;

/* This doesn't strictly conform.  10 points to anyone who guesses why!  */

int
atexit (void (*func)(void)) {
  unsigned int nfuncs;
  void (**list)(void);
  
  if (exitfuncsH) {
    UInt32 len = MemHandleSize (exitfuncsH);
    if (MemHandleResize (exitfuncsH, len + sizeof (void (*)(void))) == 0)
      nfuncs = len / sizeof (void (*)());
    else
      return 1;
    }
  else {
    if ((exitfuncsH = MemHandleNew (sizeof (void (*)(void)))) != NULL)
      nfuncs = 0;
    else
      return 1;
    }

  list = MemHandleLock (exitfuncsH);
  list[nfuncs] = func;
  MemHandleUnlock (exitfuncsH);
  return 0;
  }

static void call_exit_funcs () __attribute__ ((destructor, unused));

static void
call_exit_funcs () {
  if (exitfuncsH) {
    unsigned int nfuncs = MemHandleSize (exitfuncsH) / sizeof (void (*)(void));
    void (**list)(void) = MemHandleLock (exitfuncsH);
    void (**listlim)(void) = list + nfuncs;

    for (; list < listlim; list++)  (*list)();

    MemHandleUnlock (exitfuncsH);
    MemHandleFree (exitfuncsH);
    exitfuncsH = NULL;
    }
  }
