/* memstring.c: ISO/IEC 9899:1999  7.21  String handling -- the mem* functions.

   Placed in the public domain by John Marshall.  */

#include "string.h"

#ifdef __m68k__
#define alignment(p)  ((unsigned long) (p) & 1)
#define while_maybe   if
#else
#define alignment(p)  ((unsigned long) (p) & 3)
#define while_maybe   while
#endif

#ifdef Lmemcpy

void *
memcpy (void *vdst, const void *vsrc, size_t n) {
  char *dst = vdst;
  const char *src = vsrc;

  if (n >= 11 && alignment (dst) == alignment (src)) {
    long *ldst;
    const long *lsrc;
    size_t nl;

    while_maybe (alignment (src) != 0)  *dst++ = *src++, n--;

    ldst = (long *) dst;
    lsrc = (const long *) src;
    nl = n / 4;

    while (nl--)  *ldst++ = *lsrc++;

    dst = (char *) ldst;
    src = (const char *) lsrc;
    n = n % 4;
    }

  while (n--)  *dst++ = *src++;

  return vdst;
  }

#endif
#ifdef Lmemset

void *
memset (void *vp, int ic, size_t n) {
  unsigned char *p = vp;
  unsigned char c = ic;

  if (n >= 11) {
    unsigned short cs;
    unsigned long cl, *pl;
    size_t nl;

    while_maybe (alignment (p) != 0)
      *p++ = c, n--;

    cs = c;
    cs |= c << 8;
    cl = cs;
    cl |= cl << 16;

    pl = (unsigned long *) p;
    nl = n / 4;

    while (nl--)  *pl++ = cl;

    p = (unsigned char *) pl;
    n = n % 4;
    }

  while (n--)  *p++ = c;

  return vp;
  }

#endif
