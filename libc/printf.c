#ifdef BOOTSTRAP
#include "bootstrap.h"
#else
#include <StringMgr.h>
#endif

#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

int printf(const char *fmt, ...)
{
  va_list args;
  int i;
  int j;
  char buf[128];

  va_start(args, fmt);
  i=vsprintf(buf,fmt,args);
  va_end(args);
  for (j=0;j<StrLen(buf);j++) {
    putchar(buf[j]);
  }
  return i;
}
