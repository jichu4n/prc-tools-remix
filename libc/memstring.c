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
#ifdef Lmemmove

void *
memmove (void *vdest, const void *vsrc, size_t n) {
  union { void *v; char *c; long *l; } dest = { vdest };
  union { const char *c; const long *l; } src = { vsrc };

  if (dest.c < src.c)
    return memcpy (vdest, vsrc, n);
  else {
    dest.c += n;
    src.c += n;

    if (n >= 11 && alignment (dest.c) == alignment (src.c)) {
      size_t nl;

      while_maybe (alignment (src.c) != 0)  *--dest.c = *--src.c, n--;

      nl = n / 4;
      n = n % 4;

      while (nl--)  *--dest.l = *--src.l;
      }

    while (n--)  *--dest.c = *--src.c;

    return dest.v;
    }
  }

#endif
#ifdef Lmemcmp

int
memcmp (const void *vs1, const void *vs2, size_t n) {
  union { const unsigned char *c; const long *l; } s1 = { vs1 }, s2 = { vs2 };

  if (n >= 11 && alignment (s1.c) == alignment (s2.c)) {
    size_t nl;

    while_maybe (alignment (s1.c) != 0) {
      int diff = *s1.c++ - *s2.c++;
      if (diff != 0)
	return diff;
      n--;
      }

    nl = n / 4;
    n = n % 4;

    while (nl > 0) {
      if (*s1.l++ != *s2.l++) {
	s1.l--;
	s2.l--;
	n = 4;
	break;
	}
      nl--;
      }
    }

  while (n > 0) {
    int diff = *s1.c++ - *s2.c++;
    if (diff != 0)
      return diff;
    n--;
    }

  return 0;
  }

#endif
#ifdef Lmemchr

void *
memchr (const void *sv, int ic, size_t n) {
  const unsigned char *s = sv;
  unsigned char c = ic;

  while (n > 0) {
    if (*s++ == c)
      return (void *) --s;
    n--;
    }

  return NULL;
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
