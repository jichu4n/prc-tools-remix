/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRLEN
size_t strlen(const char * s)
{
  const char *sc;
  
  for (sc = s; *sc != '\0'; ++sc)
    /* nothing */;
  return sc - s;
}
#endif

