/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_MEMMOVE
void * memmove(void * dest,const void *src,size_t count)
{
  char *tmp, *s;

  if (dest <= src) {
    tmp = (char *) dest;
    s = (char *) src;
    while (count--)
      *tmp++ = *s++;
  }
  else {
    tmp = (char *) dest + count;
    s = (char *) src + count;
    while (count--)
      *--tmp = *--s;
  }
  
  return dest;
}
#endif

