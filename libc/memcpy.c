/* C support library memcpy() function.

   Placed in the public domain by John Marshall on 2002-08-16.  */

#include <string.h>

/* The memcpy function copies N characters from the object point to by VSRC
   into the object pointed to by VDST [and] and returns the value of VDST.
   If copying takes place between objects that overlap, the behaviour is
   undefined.  (ISO/IEC 9899:1999 7.21.2.1)  */

#ifdef __m68k__
#define alignment(p)  ((unsigned long)(p) & 1)
#define while_maybe   if
#else
#define alignment(p)  ((unsigned long)(p) & 3)
#define while_maybe   while
#endif

void *
memcpy (void *vdst, const void *vsrc, size_t n) {
  char *dst = vdst;
  const char *src = vsrc;

  if (n >= 11 && alignment (dst) == alignment (src)) {
    long *ldst;
    const long *lsrc;
    size_t nl;

    while_maybe (alignment (src) != 0)  *dst++ = *src++;

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
