/* pfdtime.c: convert host time to/from Palm OS time.

   Copyright (c) 1999 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <time.h>

#include "pfdheader.h"

/* These functions translate to/from a "Christian day number", i.e., the
   number of days since 0001-01-01.  (This is different from a julian date
   by about four and a half thousand years.)  I've previously tried very
   hard to write these functions in terms of the standard C time functions,
   but I fear it's impossible.  But I'm happy to be proved wrong...  */

static unsigned long month_offset_in_year[2][13] = {
  { 0,
    31,
    31+28,
    31+28+31,
    31+28+31+30,
    31+28+31+30+31,
    31+28+31+30+31+30,
    31+28+31+30+31+30+31,
    31+28+31+30+31+30+31+31,
    31+28+31+30+31+30+31+31+30,
    31+28+31+30+31+30+31+31+30+31,
    31+28+31+30+31+30+31+31+30+31+30,
    31+28+31+30+31+30+31+31+30+31+30+31 },
  { 0,
    31,
    31+29,
    31+29+31,
    31+29+31+30,
    31+29+31+30+31,
    31+29+31+30+31+30,
    31+29+31+30+31+30+31,
    31+29+31+30+31+30+31+31,
    31+29+31+30+31+30+31+31+30,
    31+29+31+30+31+30+31+31+30+31,
    31+29+31+30+31+30+31+31+30+31+30,
    31+29+31+30+31+30+31+31+30+31+30+31 }
  };

/* This is a host computer with essentially unlimited processing power.
   We can jolly well get the leap years right!  We're going to ignore
   leap seconds though :-).  */

static int
is_leap (unsigned long y) {
  return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
  }

static unsigned long
cdn_of_tm (const struct tm *tm) {
  unsigned long days, y;

  y = 1900 + tm->tm_year;

  days = 365 * (y-1) + (y-1) / 4 - (y-1) / 100 + (y-1) / 400;
  days += month_offset_in_year[is_leap (y)][tm->tm_mon];
  days += tm->tm_mday - 1;

  return days;
  }

#ifndef min
#define min(a,b)  (((a) < (b))? (a) : (b))
#endif

static void
tm_of_cdn (struct tm *tm, unsigned long days) {
  unsigned long y, y400, y100, y4, y1;
  int leap, i;

  /* The 1 here is a little bit magical.  */
  tm->tm_wday = (days + 1) % 7;

  y400 = days / (400 * 365 + 97);
  days %= 400 * 365 + 97;
  y100 = min (days / (100 * 365 + 24), 3);
  days -= y100 * (100 * 365 + 24);
  y4 = days / (4 * 365 + 1);
  days %= 4 * 365 + 1;
  y1 = min (days / 365, 3);
  days -= y1 * 365;

  tm->tm_yday = days;

  y = 400 * y400 + 100 * y100 + 4 * y4 + y1 + 1;

  tm->tm_year = y - 1900;
  leap = is_leap (y);

  for (i = 0; i < 12; i++)
    if (days < month_offset_in_year[leap][i+1])
      break;

  tm->tm_mon = i;
  days -= month_offset_in_year[leap][i];

  tm->tm_mday = days + 1;
  }

#undef min

/* Convert from the 0001-01-01 epoch to the one we are interested in.  For
   Palm OS, this is 1904-01-01.  */

static unsigned long epoch_bias = 695055;

unsigned long
palmostime_of_tm (const struct tm *tm) {
  return (86400ul * (cdn_of_tm (tm) - epoch_bias)
	  + 3600ul * tm->tm_hour + 60ul * tm->tm_min + tm->tm_sec);
  }

void
tm_of_palmostime (struct tm *tm, unsigned long secs) {
  tm->tm_sec = secs % 60;
  secs /= 60;
  tm->tm_min = secs % 60;
  secs /= 60;
  tm->tm_hour = secs % 24;
  secs /= 24;
  tm_of_cdn (tm, secs + epoch_bias);
  }


#ifdef TEST_DATES

#include <stdio.h>
#include <string.h>

void
set_tm (struct tm *tm, int y, int m, int d) {
  tm->tm_year = y - 1900;
  tm->tm_mon = m - 1;
  tm->tm_mday = d;
  }

void
check1 (unsigned long n) {
  struct tm tm;
  unsigned long n2;
  tm_of_cdn (&tm, n);
  n2 = cdn_of_tm (&tm);
  if (n2 != n) {
    char buf[40];
    strftime (buf, sizeof buf, "%Y-%m-%d", &tm);
    printf ("Failed %lu -> %s -> %lu\n", n, buf, n2);
    }
  }

void
check2 (unsigned long y) {
  struct tm tm;
  unsigned long dec31, jan1;

  set_tm (&tm, y, 12, 31);
  dec31 = cdn_of_tm (&tm);
  set_tm (&tm, y+1, 1, 1);
  jan1 = cdn_of_tm (&tm);

  if (jan1 != dec31 + 1)
    printf ("Failed: %lu-12-31 is %lu, %lu-01-01 is %lu\n",
	    y, dec31, y+1, jan1);
  }

int
check3 (unsigned long y) {
  struct tm tm;
  unsigned long feb28, feb29, mar1;

  set_tm (&tm, y, 2, 28);
  feb28 = cdn_of_tm (&tm);
  set_tm (&tm, y, 2, 29);
  feb29 = cdn_of_tm (&tm);
  set_tm (&tm, y, 3, 1);
  mar1 = cdn_of_tm (&tm);

  if (feb28+1 == feb29 && feb29+1 == mar1)
    return 1;
  else if (feb28+1 == feb29 && feb29 == mar1)
    return 0;

  printf ("Failed: %lu-02-28 is %lu, %lu-02-29 is %lu, %lu-03-1 is %lu\n",
	  y, feb28, y, feb29, y, mar1);

  return 0;
  }

void
show (int y, int m, int d) {
  static char *dayname[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
    };
  struct tm tm;
  char buf[40];
  unsigned long n;

  set_tm (&tm, y, m, d);
  strftime (buf, sizeof buf, "%Y-%m-%d", &tm);
  n = cdn_of_tm (&tm);
  tm_of_cdn (&tm, n);
  printf ("Check %s gives %lu is a %s, day #%d of the year\n",
	  buf, n, dayname[tm.tm_wday], tm.tm_yday);
  }

void
check_palmos (unsigned long pt, const char *title) {
  struct tm tm;
  char buf[40];
  tm_of_palmostime (&tm, pt);
  strftime (buf, sizeof buf, "%Y-%m-%d %T", &tm);
  printf ("Check 0x%08lx should be %s, is %s\n", pt, title, buf);
  }

void
check_palmos_now () {
  time_t t;
  struct tm tm;
  char buf[40];
  time (&t);
  memcpy (&tm, localtime (&t), sizeof (struct tm));
  strftime (buf, sizeof buf, "%Y-%m-%d %T (now)", &tm);
  check_palmos (palmostime_of_tm (&tm), buf);
  }

int
main () {
  unsigned long n, t;

  show (1900, 2, 28);
  show (1900, 2, 29);
  show (1900, 3, 1);
  show (1904, 1, 1);
  show (1999, 11, 15);
  show (1999, 12, 31);
  show (2000, 1, 1);
  show (2000, 2, 28);
  show (2000, 2, 29);
  show (2000, 3, 1);

  printf ("Checking year boundaries are adjacent...\n");
  t = 0;
  for (n = 1; n <= 3000; n++, t++)  check2 (n);
  printf ("Checked %lu values\n", t);

  printf ("Checking leap years from 1895 to 2105...\n");
  t = 0;
  for (n = 1895; n <= 2105; n++) {
    int ch = check3 (n);
    int really = is_leap (n);
    if (ch)  t++;
    if (!ch && really)  printf ("Failed: thought %lu isn't a leap year\n", n);
    if (ch && !really)  printf ("Failed: thought %lu is a leap year\n", n);
    }
  printf ("Checked those; found %lu leap years; expected 51\n", t);

  check_palmos (0xb3d6f4e9, "1999-8-11 9:37:45");
  check_palmos (0xb38317e3, "1999-6-8 18:56:35");
  check_palmos (0xb44e07ec, "1999-11-9 17:18:36");
  check_palmos_now ();

  printf ("Checking cdn_of_tm (tm_of_cdn (n)) == n, for all n...\n");
  t = 0;
  for (n = 0; n < 1000000; n++, t++)  check1 (n);
  for (n = 10000000; n < 12000000; n++, t++)  check1 (n);
  printf ("Checked %lu values\n", t);

  return 0;
  }

#endif
