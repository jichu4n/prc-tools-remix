/* obj-res.cpp: extract resources from a bfd executable.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.

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

static const char* version = "2.0";

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
  { NULL }
  };

static bool
no_extra_code_sections (char **namep, unsigned long *idp) {
  return false;
  }

int
main (int argc, char** argv) {
  int c, longind;
  bool work_desired = true;

  progname = argv[0];

  struct binary_file_info info;
  struct binary_file_stats stats;
  info.kind = DK_APPLICATION;
  info.maincode_id = UNSPECIFIED_RESID;
  info.emit_data = info.force_rloc = true;
  info.data_compression = 0;
  info.next_coderes = no_extra_code_sections;
  info.stats = NULL;

  while ((c = getopt_long (argc, argv, shortopts, longopts, &longind)) != -1)
    switch (c) {
    case 'l':
      info.kind = DK_GLIB;
      break;

    case 'L':
      info.kind = DK_SYSLIB;
      einfo (E_NOFILE|E_WARNING, "offset table not supported by this version");
      break;

    case 'v':
      info.stats = &stats;
      break;

    case 'z':
      info.data_compression = atoi (optarg);
      break;

    case OPTION_HELP:
      usage();
      work_desired = false;
      break;

    case OPTION_VERSION:
      printf ("%s version %s\n", progname, version);
      work_desired = false;
      break;
      }

  if (!work_desired)
    return 0;

  if (argc - optind != 1) {
    usage();
    return EXIT_FAILURE;
    }

  init_binary();

  pfd out;
  process_binary_file (out, argv[optind], info);

  if (info.stats) {
    printf ("Zeroes in .data omitted completely due to 3-part format: %ld",
	    (long) stats.omitted_zeros);
    if (stats.data_size)
      printf (" (%ld%%)", (long) (stats.omitted_zeros * 100) / stats.data_size);
    printf ("\n");
    }

  if (nerrors == 0) {
    // Write out individual resource files.
    for (const pfd_entry* entry = out[0]; entry; entry = out.succ (entry)) {
      char buffer[FILENAME_MAX];
      sprintf (buffer, "%.4s%04lx.%s.grc", 
	       entry->type(), entry->id(), argv[optind]);

      FILE *f = fopen (buffer, "wb");
      if (f) {
	fwrite (entry->contents(), 1, entry->length(), f);
	fclose (f);
	}
      else
	einfo (E_NOFILE|E_WARNING|E_PERROR, "can't write to `%s'", buffer);
      }

    return EXIT_SUCCESS;
    }
  else
    return EXIT_FAILURE;
  }
