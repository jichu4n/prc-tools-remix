/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRCAT
char * strcat(char * dest, const char * src)
{
  char *tmp = dest;
  
  while (*dest)
    dest++;
  while ((*dest++ = *src++) != '\0');

  return tmp;
}
#endif

