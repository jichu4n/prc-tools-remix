/* conio.c: Console I/O routines for Palmos/Pilot
 * (c) 1995, 1997 Dionne & Associates
 * (c) 1995 DKG Technologies
 *
 * This code is free software, under the LGPL v2
 */

#include "stdio.h"

#include <StringMgr.h>
#include <Font.h>
#include <Rect.h>
#include <Window.h>

#include "NewTypes.h"

#if SDK_VERSION < 35
#define winUp  up
#endif

#define XB 5
#define YB 12
#define YINC 10

#define XMAX 32
#define YMAX 14

static char fb[XMAX * YMAX];

static int xc;
static int yc;

static inline int DRAWLN(int LN) 
{
  FntSetFont(stdFont);
  WinDrawChars(&fb[LN * XMAX], StrLen(&fb[LN * XMAX]), XB, YB + YINC*(LN));
  return 0;
}

static int scroll()
{
  int i;
  RectangleType r = { { XB, YB }, { XB + XMAX * 4, YB + (YMAX -1) * YINC } };
  RectangleType v = { { XB, YB + (YMAX-2)*YINC }, { XB + XMAX * 4, YB + (YMAX -1) * YINC } };

  WinScrollRectangle(&r, winUp, YINC, &v);
  for (i=0;i<XMAX;) fb[(YMAX-1) * XMAX + i++]=0;
  WinEraseRectangle(&v,1);

  return YMAX-1;
}

int putchar (unsigned int buf)
{
  if (!(yc < YMAX)) yc = scroll();
  if (buf == '\n' || (!(xc < XMAX))) {
    DRAWLN(yc++);
    xc = 0;

    if (buf =='\n') return 0;
  }

  fb[yc * XMAX + xc++] = buf;
  return 0;
}

int puts (unsigned char *string)
{
  int i;

  for (i=0;i<StrLen(string);i++) {
    putchar(string[i]);
  }
  putchar('\n');
  return 0;
}
