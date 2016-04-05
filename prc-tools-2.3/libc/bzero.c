/* C support library memory functions for Palm OS.

   Author: John Marshall  (They're not very complicated :-))

   This code is in the public domain.  */

void
bzero (void *vs, int n) {
  char *s = vs;

  while (n--)
    *s++ = '\0';
  }
