/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRSTR
char * strstr(const char * s1,const char * s2)
{
  int l1, l2;
  
  l2 = strlen(s2);
  if (!l2)
    return (char *) s1;
  l1 = strlen(s1);
  while (l1 >= l2) {
    l1--;
    if (!memcmp(s1,s2,l2))
      return (char *) s1;
    s1++;
  }
  return NULL;
}
#endif
