typedef unsigned long size_t;




static __inline__ char *
in_strcpy (char *d, const char *s) {
  __asm__ ("0:move%.b (%1)+,(%0)+;bne.s 0b" : : "a" (d), "a" (s) : "cc");
  return d;
  }
