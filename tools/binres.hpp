/* binres.hpp: header file for binres.cpp.

   Copyright (c) 1998, 1999, 2001 by John Marshall.
   <jmarshall@acm.org>

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

#ifndef BINRES_HPP
#define BINRES_HPP

#include <map>
#include "pfd.hpp"

struct binary_file_info {
  // Generic information for all kinds of projects:
  ResKey maincode;
  std::map<const char*, ResKey> extracode;  // section_name -> ResKey

  // Information specific to applications:
  bool emit_appl_extras;  // code#0, pref#0, and the ori.b #1,%d0 trash
  unsigned long stack_size;

  // What to do with the data sections:
  bool emit_data, force_rloc;
  int data_compression;
  };

struct binary_file_stats {
  // Statistics from data resource compression:
  size_t data_size, omitted_zeros;
  };

ResourceDatabase process_binary_file (const char* fname,
				      const binary_file_info& info,
				      binary_file_stats* stats = NULL);

// Returns a NULL-terminated vector of target names.  Remember to free() it
// when you're finished with it.
const char** binary_file_targets ();

#endif
