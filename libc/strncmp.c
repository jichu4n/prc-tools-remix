/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>


#ifndef __HAVE_ARCH_STRNCMP
int strncmp(const char * cs,const char * ct,size_t count)
{
  register signed char __res = 0;
  
  while (count) {
    if ((__res = *cs - *ct++) != 0 || !*cs++)
      break;
    count--;
  }
  
  return __res;
}
#endif

