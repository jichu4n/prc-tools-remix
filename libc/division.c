/* C support library division functions for Palm OS.

   Author: John Marshall  (They're not very complicated :-))

   This code is in the public domain.  */

#include "stdlib.h"

#ifdef Ldiv

div_t
div (int numer, int denom) {
  div_t res;
  res.quot = numer / denom;
  res.rem = numer % denom;
  return res;
  }

#endif
#ifdef Lldiv

ldiv_t
ldiv (long numer, long denom) {
  ldiv_t res;
  res.quot = numer / denom;
  res.rem = numer % denom;
  return res;
  }

#endif
