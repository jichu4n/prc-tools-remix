/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRNLEN
size_t strnlen(const char * s, size_t count)
{
  const char *sc;
  
  for (sc = s; count-- && *sc != '\0'; ++sc)
    /* nothing */;
  return sc - s;
}
#endif

