/* abs.c: ISO/IEC 9899:1999  7.20.6.1  The abs, labs, and llabs functions.

   Placed in the public domain by John Marshall.  */

#include "stdlib.h"

#ifdef Labs

int
abs (int j) {
  return (j >= 0)? +j : -j;
  }

#endif
#ifdef Llabs

long int
labs (long int j) {
  return (j >= 0)? +j : -j;
  }

#endif
#ifdef Lllabs

long long int
llabs (long long int j) {
  return (j >= 0)? +j : -j;
  }

#endif
