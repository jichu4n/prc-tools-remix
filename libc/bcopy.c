/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_BCOPY
#undef bcopy
char * bcopy(const char * src, char * dest, int count)
{
  char *tmp = dest;
  
  while (count--)
    *tmp++ = *src++;
  
  return dest;
}
#endif

