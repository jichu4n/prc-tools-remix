#include <System/StringMgr.h>

#include "string.h"

char *a, *b;

int x1x, x2x;

extern void g();

void f() {
  x2x = strlen(a);
  }

void f2() {
  x2x = StrLen(a);
  }

void f3() {
  in_strcpy (a, b);
  x1x++;
  __builtin_strcpy (a, b);
  x1x++;
  StrCopy (a, b);
  }

char *dest, *src;

void ff1() {
  in_strcpy (dest, src);
  }

void ff2() {
  StrCopy (dest, src);
  in_strcpy (a, b);
  }

/*
char *h() {
  __builtin_strcpy (a, b);

  g();

  return __builtin_strcpy (b, a);
  }
*/

char xc;
int xi;
long xl;

void q() {
  xc = abs(xc);
  xi = abs(xi);
  xl = labs(xl);
  }
