#ifndef _STDLIB_H_
#define _STDLIB_H_

#define __need_size_t
#define __need_NULL
#include "stddef.h"

/* for sysRandomMax */
#include "System/SysUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Memory allocation */

extern void *malloc (size_t __size);
extern void free (void *__ptr);
extern void *calloc (size_t __nmemb, size_t __memb_size);
extern void *realloc (void *__ptr, size_t __size);

extern void *__MemPtrNew (unsigned long __size)
  __attribute__ ((__systrap__ (0xa013)));

extern short __MemPtrFree (void *__ptr)
  __attribute__ ((__systrap__ (0xa012)));

extern __inline__ void *
malloc (size_t __size) {
  return __MemPtrNew (__size);
  }

extern __inline__ void
free (void *__ptr) {
  if (__ptr)  __MemPtrFree (__ptr);
  }

/* String conversions */

extern int atoi (const char *__ptr);
extern long atol (const char *__ptr);
extern long strtol (const char *__ptr, char **__ptrlim, int __base);
extern unsigned long strtoul (const char *__ptr, char **__ptrlim, int __base);

/* Anyone want to choose a float represntation for atof() and strtod()?  */

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

/* Multi-byte characters */

#ifdef __cplusplus
}
#endif

#endif
