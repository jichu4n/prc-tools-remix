/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRCMP
int strcmp(const char * cs1,const char * ct1)
{
  const unsigned char * cs = (unsigned char *)cs1;
  const unsigned char * ct = (unsigned char *)ct1;
  register int __res;
  
  while (1) {
    if ((__res = *cs - *ct++) != 0 || !*cs++)
      break;
  }
  
  return __res;
}
#endif

