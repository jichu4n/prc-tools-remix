/* stdlib.h: ISO/IEC 9899:1999  7.20  General utilities.

   Placed in the public domain by John Marshall.

   This file is part of prc-tools, but is not licensed in the same way
   as the majority of prc-tools.  The contents of this file, and also
   the implementations of the functions declared in this file, are in
   the public domain.

   One particular consequence of this is that you may use these functions
   in Palm OS executables without concern that doing so by itself causes
   the resulting executable to be covered by any particular license, such as
   the GNU General Public License that covers much of the rest of prc-tools.
   However this does not invalidate any other reasons why the executable
   might be covered by such a license (such as the use of other functions
   from other header files that are so-covered).  */

#ifndef _PRC_TOOLS_STDLIB_H
#define _PRC_TOOLS_STDLIB_H

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 7.20.1  Numeric conversion functions [partial; integer only, no errno].  */

int atoi (const char *_nptr);
long int atol (const char *_nptr);
long long int atoll (const char *_nptr);

/* Note that this implementation of the strto* functions does clip their
   return values to the bounds allowed by their respective return types,
   but it DOES NOT signal those overflows via errno.  */

long int strtol (const char *_nptr, char **_endptr, int _base);
long long int strtoll (const char *_nptr, char **_endptr, int _base);
unsigned long int strtoul (const char *_nptr, char **_endptr, int _base);
unsigned long long int strtoull (const char *_nptr, char **_endptr, int _base);

#ifdef __OPTIMIZE__
extern __inline__ int atoi (const char *_nptr) { return (int) atol (_nptr); }
#endif

/* 7.20.3  Memory management functions.  */

/* FIXME Add __attribute__ ((__malloc__)) to these when we update to GCC 3.  */

void *malloc (size_t _size);
void *calloc (size_t _nmemb, size_t _size);
void *realloc (void *_ptr, size_t _size);
void free (void *_ptr);

/* 7.20.4  Communication with the environment [partial].  */

void abort (void)  __attribute__ ((__noreturn__));
int atexit (void (*_func) (void));

/* 7.20.6  Integer arithmetic functions.  */

int abs (int _j)			__attribute__ ((__const__));
long int labs (long int _j)		__attribute__ ((__const__));
long long int llabs (long long int _j)	__attribute__ ((__const__));

typedef struct { int rem, quot; } div_t;
typedef struct { long int rem, quot; } ldiv_t;
typedef struct { long long int rem, quot; } lldiv_t;

div_t div (int _numer, int _denom)		__attribute__ ((__const__));
ldiv_t ldiv (long int _numer, long int _denom)	__attribute__ ((__const__));
lldiv_t lldiv (long long int _numer, long long int _denom)
						__attribute__ ((__const__));

#ifdef __cplusplus
}
#endif

#endif
