/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRTOUL
unsigned long strtoul(const char *cp,char **endp,unsigned int base)
{
  unsigned long result = 0,value;
  
  if (!base) {
    base = 10;
    if (*cp == '0') {
      base = 8;
      cp++;
      if ((*cp == 'x') && isxdigit(cp[1])) {
	cp++;
	base = 16;
      }
    }
  }
  while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp) ? toupper(*cp) : *cp)-'A'+10) < base) {
    result = result*base + value;
    cp++;
  }
  if (endp)
    *endp = (char *)cp;
  return result;
}
#endif
