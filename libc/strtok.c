/*
 *  linux/lib/string.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#ifndef __HAVE_ARCH_STRTOK
char * ___strtok = NULL;

char * strtok(char * s,const char * ct)
{
  char *sbegin, *send;
  
  sbegin  = s ? s : ___strtok;
  if (!sbegin) {
    return NULL;
  }
  sbegin += strspn(sbegin,ct);
  if (*sbegin == '\0') {
    ___strtok = NULL;
    return( NULL );
  }
  send = strpbrk( sbegin, ct);
  if (send && *send != '\0')
    *send++ = '\0';
  ___strtok = send;
  return (sbegin);
}
#endif

