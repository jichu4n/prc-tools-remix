/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>


#ifndef __HAVE_ARCH_STRTOL
signed long strtol(const char *cp,char **endp,unsigned int base)
{
  signed long sign;
  signed long result = 0,value;
  if (!isdigit(*cp))
    {
      if ((*cp) == '-')
	sign = -1;
      else
	sign = 1;
      cp++;
    }
  else sign = 1;
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
  while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
							     ? toupper(*cp) : *cp)-'A'+10) < base) {
    result = result*base + value;
    cp++;
  }
  if (endp)
    *endp = (char *)cp;
  return (result * sign);
}
#endif
