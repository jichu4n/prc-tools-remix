/* string.h: ISO/IEC 9899:1999  7.21  String handling.

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

#ifndef _PRC_TOOLS_STRING_H
#define _PRC_TOOLS_STRING_H

#define __need_size_t
#define __need_NULL
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 7.21.2  Copying functions.  */

void *memcpy  (void *_dest, const void *_src, size_t _n);
void *memmove (void *_dest, const void *_src, size_t _n);
char *strcpy  (char *_dest, const char *_src);
char *strncpy (char *_dest, const char *_src, size_t _n);

/* 7.21.3  Concatenation functions.  */

char *strcat  (char *_dest, const char *_src);
char *strncat (char *_dest, const char *_src, size_t _n);

/* 7.21.4  Comparison functions [partial; no strcoll(), no strxfrm()].  */

int memcmp  (const void *_s1, const void *_s2, size_t _n);
int strcmp  (const char *_s1, const char *_s2);
int strncmp (const char *_s1, const char *_s2, size_t _n);

/* 7.21.5  Search functions.  */

void *memchr   (const void *_s, int _c, size_t _n);
char *strchr   (const char *_s, int _c);
size_t strcspn (const char *_s, const char *_reject);
char *strpbrk  (const char *_s, const char *_accept);
char *strrchr  (const char *_s, int _c);
size_t strspn  (const char *_s, const char *_accept);
char *strstr   (const char *_s, const char *_key);
char *strtok   (char *_s, const char *_delim);

/* 7.21.6  Miscellaneous functions [partial; no strerror()].  */

void *memset  (void *_s, int _c, size_t _n);
size_t strlen (const char *_s);

#ifdef __cplusplus
}
#endif

#endif
