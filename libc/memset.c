/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_MEMSET
void * memset(void * s,char c,size_t count)
{
  char *xs = (char *) s;

  while (count--)
    *xs++ = c;
  
  return s;
}
#endif

