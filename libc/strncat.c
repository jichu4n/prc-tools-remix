/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRNCAT
char * strncat(char *dest, const char *src, size_t count)
{
  char *tmp = dest;
  
  if (count) {
    while (*dest)
      dest++;
    while ((*dest++ = *src++)) {
      if (--count == 0)
	break;
    }
  }
  
  return tmp;
}
#endif
