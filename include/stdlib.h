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

/* 7.20.3  Memory management functions.  */

/* FIXME Add __attribute__ ((__malloc__)) to these when we update to GCC 3.  */

void *malloc (size_t _size);
void *calloc (size_t _nmemb, size_t _size);
void *realloc (void *_ptr, size_t _size);
void free (void *_ptr);

/* 7.20.4  Communication with the environment [partial].  */

void abort (void)  __attribute__ ((__noreturn__));
int atexit (void (*_func) (void));

#ifdef __cplusplus
}
#endif

#endif
