/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRNCPY
char * strncpy(char * dest,const char *src,size_t count)
{
  char *tmp = dest;
  
  while (count-- && (*dest++ = *src++) != '\0')
    /* nothing */;
  
  return tmp;
}
#endif

