/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_MEMCPY
void * memcpy(void * dest,const void *src,size_t count)
{
  char *tmp = (char *) dest, *s = (char *) src;
  
  while (count--)
    *tmp++ = *s++;
  
  return dest;
}
#endif

