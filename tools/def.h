/* def.h: header file for .def file parsing.

   Copyright 2002 John Marshall.
   Portions copyright 1998, 1999 Palm, Inc. or its subsidiaries.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

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

#ifndef DEF_H
#define DEF_H

enum database_kind {
  DK_APPLICATION,
  DK_GLIB,
  DK_SYSLIB,
  DK_HACK,
  DK_GENERIC
  };

struct database_header;

struct def_callbacks {
  void (*db_header)(enum database_kind kind, const struct database_header *h);
  void (*multicode_section)(const char *secname);
  void (*export_function)(const char *funcname);
  void (*data)(int allow);
  void (*stack)(unsigned long);
  void (*version_resource)(unsigned long resno, const char *text);
  };

extern struct def_callbacks default_def_callbacks;

#ifdef __cplusplus
extern "C" {
#endif

void read_def_file (const char *fname, const struct def_callbacks *callbacks);

#ifdef __cplusplus
}
#endif

#endif
