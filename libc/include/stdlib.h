/* This file is part of prc-tools.  */

#ifndef _PRC_TOOLS_STDLIB_H
#define _PRC_TOOLS_STDLIB_H

/* FIXME As we replace the somewhat borked functions from the old prc-tools
   sources, we'll move their declarations to this section.  Eventually the
   section below will disappear.  */

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifndef sysRandomMax
#include <SysUtils.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Memory allocation */

void *malloc (size_t _size);
void free (void *_ptr);
void *calloc (size_t _nmemb, size_t _memb_size);
void *realloc (void *_ptr, size_t _size);

/* FIXME Above here has been updated.  */

/* String conversions */

extern int atoi (const char *__ptr);
extern long atol (const char *__ptr);
extern long strtol (const char *__ptr, char **__ptrlim, int __base);
extern unsigned long strtoul (const char *__ptr, char **__ptrlim, int __base);

/* Anyone want to choose a float representation for atof() and strtod()?  */

/* Division */

typedef struct { int rem, quot; } div_t;
extern div_t div (int __numer, int __denom) __attribute__ ((__const__));

extern __inline__ div_t
div (int __numer, int __denom) {
  div_t __res;
  __asm__ ("divs.w %1,%0" : "=a" (__res) : "g" (__denom), "0" (__numer) : "cc");
  return __res;
  }

typedef struct { long rem, quot; } ldiv_t;
extern ldiv_t ldiv (long __numer, long __denom) __attribute__ ((__const__));

/* Random numbers */

#define RAND_MAX  (sysRandomMax)

extern int rand (void);
extern void srand (unsigned int __seed);

/* System functions */

extern int atexit (void (*__func)(void));

/* Multi-byte characters */

#ifdef __cplusplus
}
#endif

#endif
