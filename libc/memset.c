/* C support library memset() function.

   Placed in the public domain by John Marshall on 2002-08-02.  */

#include <string.h>

/* The memset function copies the value of IC (converted to an unsigned char)
   into each of the first N characters of the object pointed to by VP, [and]
   returns the value of VP.  (ISO/IEC 9899:1999 7.21.6.1)  */

void *
memset (void *vp, int ic, size_t n) {
  unsigned char *p = vp;
  unsigned char c = ic;

  if (n >= 11) {
    unsigned short cs;
    unsigned long cl, *pl;
    size_t nl;

#ifdef __m68k__
    if (((unsigned long) p) & 1)  *p++ = c, n--;
#else
    while (((unsigned long) p) & 3)  *p++ = c, n--;
#endif

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
