/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

/*
 * find the first occurrence of byte 'c', or 1 past the area if none
 */
#ifndef __HAVE_ARCH_MEMSCAN
void * memscan(void * addr, int c, size_t size)
{
  unsigned char * p = (unsigned char *) addr;

  while (size) {
    if (*p == c)
      return (void *) p;
    p++;
    size--;
  }
  return (void *) p;
}
#endif

