/* string.c: ISO/IEC 9899:1999  7.21  String handling -- the str* functions.

   Placed in the public domain by John Marshall.  */

#include "string.h"

#ifdef Lstrcpy

char *
strcpy (char *dest, const char *src) {
  char *d = dest;

  while ((*d++ = *src++) != '\0')
    ;

  return dest;
  }

#endif
#ifdef Lstrncpy

char *
strncpy (char *dest, const char *src, size_t n) {
  char *d = dest;
  char c;

  while (n > 0 && (c = *src++) != '\0')
    *d++ = c, n--;

  while (n > 0)
    *d++ = '\0', n--;

  return dest;
  }

#endif
#ifdef Lstrcat

char *
strcat (char *dest, const char *src) {
  char *d = dest;

  while (*d++ != '\0')
    ;
  d--;

  while ((*d++ = *src++) != '\0')
    ;

  return dest;
  }

#endif
#ifdef Lstrncat

char *
strncat (char *dest, const char *src, size_t n) {
  char *d = dest;
  char c;

  while (*d++ != '\0')
    ;
  d--;

  while (n > 0 && (c = *src++) != '\0')
    *d++ = c, n--;

  *d = '\0';
  return dest;
  }

#endif
#ifdef Lstrcmp

int
strcmp (const char *s1, const char *s2) {
  while (1) {
    int c1 = (unsigned char) *s1++;
    int c2 = (unsigned char) *s2++;
    int diff;

    if (c2 == 0)
      return c1;

    diff = c1 - c2;
    if (diff != 0)
      return diff;
    }
  }

#endif
#ifdef Lstrncmp

int
strncmp (const char *s1, const char *s2, size_t n) {
  while (n > 0) {
    int c1 = (unsigned char) *s1++;
    int c2 = (unsigned char) *s2++;
    int diff;

    if (c2 == 0)
      return c1;

    diff = c1 - c2;
    if (diff != 0)
      return diff;

    n--;
    }

  return 0;
  }

#endif
#ifdef Lstrchr

char *
strchr (const char *s, int ic) {
  char c = ic;

  while (*s != c)
    if (*s++ == '\0')
      return NULL;

  return (char *) s;
  }

#endif
#ifdef Lstrcspn

size_t
strcspn (const char *s, const char *reject) {
  const char *start = s;

  while (1) {
    char c = *s++;
    const char *rejs = reject;
    char rejc;

    do {
      rejc = *rejs++;
      if (c == rejc)
	return --s - start;
      } while (rejc != '\0');
    }
  }

#endif
#ifdef Lstrpbrk

char *
strpbrk (const char *s, const char *accept) {
  char c, ac;

  while ((c = *s++) != '\0') {
    const char *accs = accept;
    while ((ac = *accs++) != '\0')
      if (c == ac)
	return (char *) --s;
    }

  return NULL;
  }

#endif
#ifdef Lstrrchr

char *
strrchr (const char *s, int ic) {
  char c = ic;
  const char *location = NULL;

  do {
    if (*s == c)
      location = s;
    } while (*s++ != '\0');

  return (char *) location;
  }

#endif
#ifdef Lstrspn

size_t
strspn (const char *s, const char *accept) {
  const char *start = s;

  while (1) {
    char c = *s++;
    const char *accs = accept;
    char ac;

    do {
      ac = *accs++;
      if (ac == '\0')
	return --s - start;
      } while (c != ac);
    }
  }

#endif
#ifdef Lstrstr

char *
strstr (const char *haystack, const char *needle) {
  do {
    const char *s = haystack;
    const char *n = needle;
    char sc, nc;

    do {
      sc = *s++;
      nc = *n++;
      if (nc == '\0')
	return (char *) haystack;
      } while (sc == nc);
    } while (*haystack++ != '\0');

  return NULL;
  }

#endif
#ifdef Lstrtok

char *
strtok (char *s, const char *delim) {
  static char *saveds;

  char *token = NULL;

  if (s == NULL)
    s = saveds;

  s += strspn (s, delim);

  if (*s) {
    token = s;
    s += strcspn (s, delim);
    if (*s)
      *s++ = '\0';
    }

  saveds = s;
  return token;
  }

#endif
#ifdef Lstrlen

size_t
strlen (const char *s) {
  const char *start = s;

  while (*s++ != '\0')
    ;

  return --s - start;
  }

#endif
