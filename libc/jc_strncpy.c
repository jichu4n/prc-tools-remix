#if 0
#include "string.h"
#else
typedef unsigned long size_t;
#endif

char *
strncpy (char *d, const char *s, size_t n) {
  char *dest = d;

  while (n--)
    *d++ = (*s)? *s++ : '\0';

  return dest;
  }
