/* obj-res.cpp: extract resources from a bfd executable.

   Copyright (c) 1998, 1999 by John Marshall.
   <jmarshall@acm.org>

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program, obj-res v2.0, is mostly compatible with the old obj-res, the
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
 */

#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "bfd.h"

#include "binres.hpp"
#include "pfd.hpp"
#include "utils.h"

const char* progversion = "2.0";

static void
usage() {
  printf ("Usage: %s [-l] [-L export.file] [-v] [-z #] bfd.file\n", progname);
  printf ("Options:\n");
  propt_tab = 16;
  propt ("-l", "Generate GLib resources");
  propt ("-L EXPORT.FILE",
	 "Generate SysLib resources (EXPORT.FILE is unsupported)");
  propt ("-v", "Show statistics");
  propt ("-z NUM", "Set data compression level (0--7; by default, 0)");
  }

enum {
  OPTION_HELP = 150,
  OPTION_VERSION
  };

static char* shortopts = "lL:vz:";

static struct option longopts[] = {
  { "help", no_argument, NULL, OPTION_HELP },
  { "version", no_argument, NULL, OPTION_VERSION },
  { NULL, no_argument, NULL, 0 }
  };

int
main (int argc, char** argv) {
  int c, longind;
  bool work_desired = true;
  bool verbose = false;

  progname = argv[0];

  struct binary_file_info info;

  // By default, make an application:

  info.maincode = ResKey ("code", 1);
  // info.extracode was constructed as empty
  info.emit_appl_extras = true;
  info.stack_size = 4096;

  info.emit_data = info.force_rloc = true;
  info.data_compression = 0;

  while ((c = getopt_long (argc, argv, shortopts, longopts, &longind)) != -1)
    switch (c) {
    case 'l':
      info.maincode = ResKey ("GLib", 0);
      info.emit_appl_extras = false;
      break;

    case 'L':
      info.maincode = ResKey ("libr", 0);
      info.emit_appl_extras = false;
      einfo (E_NOFILE|E_WARNING, "offset table not supported by this version");
      break;

    case 'v':
      verbose = true;
      break;

    case 'z':
      info.data_compression = atoi (optarg);
      break;

    case OPTION_HELP:
      usage();
      work_desired = false;
      break;

    case OPTION_VERSION:
      printf ("%s version %s\n", progname, progversion);
      work_desired = false;
      break;
      }

  if (!work_desired)
    return EXIT_SUCCESS;

  if (argc - optind != 1) {
    usage();
    return EXIT_FAILURE;
    }

  struct binary_file_stats stats;
  ResourceDatabase out = process_binary_file (argv[optind], info,
					      (verbose)? &stats : NULL);

  if (verbose) {
    printf ("Zeroes in .data omitted completely due to 3-part format: %ld",
	    (long) stats.omitted_zeros);
    if (stats.data_size)
      printf (" (%d%%)", int((stats.omitted_zeros * 100) / stats.data_size));
    printf ("\n");
    }

  if (nerrors == 0) {
    char *basename = basename_with_changed_extension (argv[optind], NULL);
    for (ResourceDatabase::const_iterator it = out.begin();
	 it != out.end();
	 ++it) {
      // Write out individual resource files.
      char fname[FILENAME_MAX];
      sprintf (fname, "%.4s%04hx.%s.grc", 
	       (*it).first.type, (*it).first.id, basename);

      FILE *f = fopen (fname, "wb");
      if (f) {
	size_t len = (*it).second.size ();
	if (fwrite ((*it).second.contents (), 1, len, f) != len)
	  einfo (E_NOFILE | E_PERROR, "error writing to `%s'", fname);
	fclose (f);
	}
      else
	einfo (E_NOFILE | E_PERROR, "can't write to `%s'", fname);
      }
    }

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
