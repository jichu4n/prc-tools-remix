/* build-prc.cpp: build a .prc from a pile of files.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.

   This program, build-prc v2.0, follows in the footsteps of obj-res and
   build-prc, the source code of which contains the following notices:

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "getopt.h"

// Uh oh... this ought to go
#include "pi-file.h"

#include "utils.h"

static const char* version = "2.0";

void
usage() {
  printf ("\
Usage: Old-style: %s [options] outfile.prc 'App Name' apid file...\n",
	  progname);
  printf ("       New-style: %s [options] file...\n", progname);
  printf ("\
Files may be .bin, .grc, .prc, .def (new-style only), or linked executables,\n\
and may specify `f=#' to renumber and `f(type[#[-#]][,...])' to select\n");

  printf ("Options:\n");
  propt ("-o FILE, --output FILE",
	 "Set output .prc file name (new-style only)");
  propt ("-l, -L", "Build a GLib (SysLib)");
  propt ("-a FILE, --appinfo FILE", "Add an AppInfoBlock");
  propt ("-s FILE, --sortinfo FILE", "Add a SortInfoBlock");
  propt ("-t TYPE, --type TYPE", "Set database type");
  propt ("-c CRID, --creator CRID", "Set database creator (new-style only)");
  propt ("-n NAME, --name NAME", "Set database name (new-style only)");
  propt ("-m NUM, --modification-number NUM",
	 "Set database modification number");
  propt ("-v NUM, --version-number NUM", "Set database version number");
  propt ("--readonly, --read-only, --appinfodirty, --appinfo-dirty, --backup",
	 NULL);
  propt ("--oktoinstallnewer, --ok-to-install-newer, --resetafterinstall",
	 NULL);
  propt ("--reset-after-install, --copyprevention, --copy-prevention, --stream",
	 "Set database attributes");
  propt ("-z N, --compress-data N",
	 "Set data resource compression method (0--2)");
  propt ("-x, --provenance", "Output resource cross-reference");
  }

enum {
  OPTION_READONLY = 150,
  OPTION_APPINFO_DIRTY,
  OPTION_BACKUP,
  OPTION_OK_TO_INSTALL_NEWER,
  OPTION_RESET_AFTER_INSTALL,
  OPTION_COPY_PREVENTION,
  OPTION_STREAM,
  OPTION_HELP,
  OPTION_VERSION
  };

static char* shortopts = "o:lLa:s:t:c:n:m:v:";

static struct option longopts[] = {
  { "output", required_argument, NULL, 'o' },
  { "appinfo", required_argument, NULL, 'a' },
  { "sortinfo", required_argument, NULL, 's' },
  { "type", required_argument, NULL, 't' },
  { "creator", required_argument, NULL, 'c' },
  { "name", required_argument, NULL, 'n' },
  { "modification-number", required_argument, NULL, 'm' },
  { "version-number", required_argument, NULL, 'v' },
  { "type", required_argument, NULL, 't' },
  { "compress-data", required_argument, NULL, 'z' },

  { "readonly", no_argument, NULL, OPTION_READONLY },
  { "read-only", no_argument, NULL, OPTION_READONLY },
  { "appinfodirty", no_argument, NULL, OPTION_APPINFO_DIRTY },
  { "appinfo-dirty", no_argument, NULL, OPTION_APPINFO_DIRTY },
  { "backup", no_argument, NULL, OPTION_BACKUP },
  { "oktoinstallnewer", no_argument, NULL, OPTION_OK_TO_INSTALL_NEWER },
  { "ok-to-install-newer", no_argument, NULL, OPTION_OK_TO_INSTALL_NEWER },
  { "reset-after-install", no_argument, NULL, OPTION_RESET_AFTER_INSTALL },
  { "resetafterinstall", no_argument, NULL, OPTION_RESET_AFTER_INSTALL },
  { "copy-prevention", no_argument, NULL, OPTION_COPY_PREVENTION },
  { "copyprevention", no_argument, NULL, OPTION_COPY_PREVENTION },
  { "stream", no_argument, NULL, OPTION_STREAM },

  { "help", no_argument, NULL, OPTION_HELP },
  { "version", no_argument, NULL, OPTION_VERSION },
  { NULL }
  };

unsigned int opt_flags = 0;

bool opt_version_present = false;
/*
bool opt_version_present = false;
bool opt_version_present = false;
bool opt_version_present = false;
*/

char* opt_output_fname = NULL;
char* opt_dbname = NULL;
char* opt_dbtype = NULL;
char* opt_creator = NULL;

unsigned long opt_modno;
unsigned int opt_version;

bool opt_modno_present = false;

char* appinfo_fname = NULL;
char* sortinfo_fname = NULL;

int
main (int argc, char** argv) {
  int c, longind;
  bool work_desired = true;

  progname = argv[0];

  while ((c = getopt_long (argc, argv, shortopts, longopts, &longind)) != -1)
    switch (c) {
    case 'o':
      opt_output_fname = optarg;
      break;

    case 'l':
      opt_dbtype = "GLib";
      break;

    case 'L':
      opt_dbtype = "libr";
      break;

    case 'a':
      appinfo_fname = optarg;
      break;

    case 's':
      sortinfo_fname = optarg;
      break;

    case 't':
      opt_dbtype = optarg;
      break;

    case 'c':
      opt_creator = optarg;
      break;

    case 'n':
      opt_dbname = optarg;
      break;

    case 'm':
      opt_modno = strtoul (optarg, NULL, 0);
      opt_modno_present = true;
      break;

    case 'v':
      opt_version = (unsigned int) strtoul (optarg, NULL, 0);
      opt_version_present = true;
      break;

    case OPTION_READONLY:
      opt_flags |= dlpDBFlagReadOnly;
      break;

    case OPTION_APPINFO_DIRTY:
      opt_flags |= dlpDBFlagAppInfoDirty;
      break;

    case OPTION_BACKUP:
      opt_flags |= dlpDBFlagBackup;
      break;

    case OPTION_OK_TO_INSTALL_NEWER:
      opt_flags |= dlpDBFlagNewer;
      break;

    case OPTION_RESET_AFTER_INSTALL:
      opt_flags |= dlpDBFlagReset;
      break;

    case OPTION_COPY_PREVENTION:
      opt_flags |= dlpDBFlagCopyPrevention;
      break;

    case OPTION_STREAM:
      opt_flags |= dlpDBFlagStream;
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

// Can't use `--creator' with old-style parameters
// Can't use `--name' with old-style parameters

  usage();
  return 0;
  }
