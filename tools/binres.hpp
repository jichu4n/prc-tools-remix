/* binres.hpp: header file for binres.cpp.

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

#ifndef _BINRES_HPP_
#define _BINRES_HPP_

void init_binary();

struct binary_file_stats {
  size_t data_size, omitted_zeros;
  };

// For database_kind:
#include "utils.h"

struct binary_file_info {
  database_kind kind;
  unsigned long maincode_id;
  bool emit_data, force_rloc;
  int data_compression;
  bool (*next_coderes)(char **namep, unsigned long *idp);
  binary_file_stats* stats;
  };

const unsigned long UNSPECIFIED_RESID = ~0ul;

class pfd;

void process_binary_file (pfd& outpfd, char *fname,
			  const binary_file_info& info);

#endif
