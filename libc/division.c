/* division.c: ISO/IEC 9899:1999  7.20.6.2  The div, ldiv, and lldiv functions.

   Placed in the public domain by John Marshall.  */

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
ldiv (long int numer, long int denom) {
  ldiv_t res;
  res.quot = numer / denom;
  res.rem = numer % denom;
  return res;
  }

#endif
#ifdef Llldiv

lldiv_t
lldiv (long long int numer, long long int denom) {
  lldiv_t res;
  res.quot = numer / denom;
  res.rem = numer % denom;
  return res;
  }

#endif
