/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRSPN
size_t strspn(const char *s, const char *accept)
{
  const char *p;
  const char *a;
  size_t count = 0;
  
  for (p = s; *p != '\0'; ++p) {
    for (a = accept; *a != '\0'; ++a) {
      if (*p == *a)
	break;
    }
    if (*a == '\0')
      return count;
    ++count;
  }

  return count;
}
#endif

