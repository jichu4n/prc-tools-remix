/* def.h: header file for .def file parsing.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.

   This program follows in the footsteps of obj-res and build-prc, the
   source code of which contains the following notices:

 * obj-res.c:  Dump out .prc compatible binary resource files from an object
 *
 * (c) 1996, 1997 Dionne & Associates
 * jeff@ryeham.ee.ryerson.ca
 *
 * This is Free Software, under the GNU Public Licence v2 or greater.
 *
 * Relocation added March 1997, Kresten Krab Thorup 
 * krab@california.daimi.aau.dk

 * ptst.c:  build a .prc from a pile of files.
 *
 * (c) 1996, Dionne & Associates
 * (c) 1997, The Silver Hammer Group Ltd.
 * This is Free Software, under the GNU Public Licence v2 or greater.
 */

#ifndef _DEF_H_
#define _DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

struct def_database_info {
  struct DBInfo db;
  enum database_kind kind;
  int type_specified;  /* 0 if it's DK_GENERIC and type=xxxx wasn't used */
  };

struct def_callbacks {
  };

int read_def_file(const char *fname, struct def_database_info *info,
		  const struct def_callbacks *callbacks);

#ifdef __cplusplus
}
#endif

#endif
