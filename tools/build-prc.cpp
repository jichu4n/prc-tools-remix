/* build-prc.cpp: build a .prc from a pile of files.

   Copyright (c) 1998-2000 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

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

#include <map>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "getopt.h"
#include "utils.h"
#include "def.h"
#include "binres.hpp"
#include "pfd.hpp"
#include "pfdio.hpp"

static const char* version = "2.1";

void
usage() {
  printf ("\
Usage: Old-style: %s [options] outfile.prc 'App Name' apid file...\n",
	  progname);
  printf ("       New-style: %s [options] file...\n", progname);
  // Commented out options are intended but not yet implemented
  printf ("\
Files may be .bin, .grc, .def (new-style only), or linked executables\n");
  /* printf ("\
Files may be .bin, .grc, .prc, .def (new-style only), or linked executables,\n\
and may specify `f=#' to renumber and `f(type[#[-#]][,...])' to select\n"); */

  printf ("Options:\n");
  propt ("-o FILE, --output FILE",
	 "Set output .prc file name (new-style only)");
  propt ("-l, -L", "Build a GLib or a system library respectively");
  propt ("-a FILE, --appinfo FILE", "Add an AppInfo block");
  propt ("-s FILE, --sortinfo FILE", "Add a SortInfo block");
  propt ("-t TYPE, --type TYPE", "Set database type");
  propt ("-c CRID, --creator CRID", "Set database creator (new-style only)");
  propt ("-n NAME, --name NAME", "Set database name (new-style only)");
  propt ("-m NUM, --modification-number NUM",
	 "Set database modification number");
  propt ("-v NUM, --version-number NUM", "Set database version number");
  propt ("--readonly, --read-only, --appinfodirty, --appinfo-dirty, --backup,",
	 NULL);
  propt ("--oktoinstallnewer, --ok-to-install-newer, --resetafterinstall,",
	 NULL);
  propt ("--reset-after-install, --copyprevention, --copy-prevention,", NULL);

  propt ("--stream, --hidden, --launchabledata, --launchable-data",
	 "Set database attributes");
  propt ("-z N, --compress-data N",
	 "Set data resource compression method (0--2)");
  // propt ("-x, --provenance", "Output resource cross-reference");
  }

enum {
  OPTION_READONLY = 150,
  OPTION_APPINFO_DIRTY,
  OPTION_BACKUP,
  OPTION_OK_TO_INSTALL_NEWER,
  OPTION_RESET_AFTER_INSTALL,
  OPTION_COPY_PREVENTION,
  OPTION_STREAM,
  OPTION_HIDDEN,
  OPTION_LAUNCHABLE_DATA,
  OPTION_HELP,
  OPTION_VERSION
  };

static char* shortopts = "o:lLa:s:t:c:n:m:v:z:";

static struct option longopts[] = {
  { "output", required_argument, NULL, 'o' },
  { "appinfo", required_argument, NULL, 'a' },
  { "sortinfo", required_argument, NULL, 's' },
  { "type", required_argument, NULL, 't' },
  { "creator", required_argument, NULL, 'c' },
  { "name", required_argument, NULL, 'n' },
  { "modification-number", required_argument, NULL, 'm' },
  { "version-number", required_argument, NULL, 'v' },
  { "provenance", no_argument, NULL, 'x' },
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
  { "hidden", no_argument, NULL, OPTION_HIDDEN },
  { "launchabledata", no_argument, NULL, OPTION_LAUNCHABLE_DATA },
  { "launchable-data", no_argument, NULL, OPTION_LAUNCHABLE_DATA },

  { "help", no_argument, NULL, OPTION_HELP },
  { "version", no_argument, NULL, OPTION_VERSION },
  { NULL, no_argument, NULL, 0 }
  };


typedef map<ResKey, string> ResourceProvenance;

static database_kind db_kind;
static ResourceDatabase db;
static ResourceProvenance prov;


void
add_resource (const char* origin, const ResKey& key, const Datablock& data) {
  ResourceProvenance::const_iterator prev_supplier = prov.find (key);
  if (prev_supplier == prov.end()) {
    db[key] = data;
    prov[key] = origin;
    }
  else {
    filename = origin;
    einfo (E_FILE | E_WARNING, "resource %.4s #%u already obtained from `%s'",
	   key.type, key.id, (*prev_supplier).second.c_str());
    }
  }


Datablock
slurp_file_as_datablock (const char* fname) {
#ifdef HAVE_FILE_LENGTH
  FILE* f = fopen (fname, "rb");
  if (f) {
    long length = file_length (f);
    Datablock block (length);
    if (fread (block.writable_contents(), 1, length, f) != size_t(length))
      einfo (E_NOFILE | E_PERROR, "error reading `%s'", fname);
    fclose (f);
    return block;
    }
  else {
    einfo (E_NOFILE | E_PERROR, "can't open `%s'", fname);
    return Datablock();
    }
#else
  long length;
  void* buffer = slurp_file (fname, "rb", &length);

  if (buffer) {
    Datablock block (length);
    memcpy (block.writable_contents(), buffer, length);
    free (buffer);
    return block;
    }
  else {
    einfo (E_NOFILE | E_PERROR, "can't read raw file `%s'", fname);
    return Datablock();
    }
#endif
  }


struct parameter_state {
  database_kind kind;
  struct database_header db;
  bool kind_present, version_present, modnum_present;

  parameter_state ();
  };

parameter_state::parameter_state () {
  init_database_header (&db);
  kind_present = version_present = modnum_present = false;
  }

void
merge_state (const parameter_state& s) {
  if (s.kind_present)  db_kind = s.kind;

  if (s.db.name[0] != '\0')  strncpy (db.name, s.db.name, 32);
  if (s.db.type[0] != '\0')  strncpy (db.type, s.db.type, 4);
  if (s.db.creator[0] != '\0')  strncpy (db.creator, s.db.creator, 4);

  db.readonly |= s.db.readonly;
  db.appinfo_dirty |= s.db.appinfo_dirty;
  db.backup |= s.db.backup;
  db.ok_to_install_newer |= s.db.ok_to_install_newer;
  db.reset_after_install |= s.db.reset_after_install;
  db.copy_prevention |= s.db.copy_prevention;
  db.stream |= s.db.stream;
  db.hidden |= s.db.hidden;
  db.launchable_data |= s.db.launchable_data;

  if (s.version_present)  db.version = s.db.version;
  if (s.modnum_present)  db.modnum = s.db.modnum;

  if (palmostime_of_tm (&s.db.created))  db.created = s.db.created;
  if (palmostime_of_tm (&s.db.modified))  db.modified = s.db.modified;
  if (palmostime_of_tm (&s.db.backedup))  db.backedup = s.db.backedup;
  }


static struct binary_file_info bininfo;


// I'm not sure I really understand how to do this, but we'll try:
extern "C" {

static void
db_header (database_kind kind, const struct database_header* h) {
  parameter_state s;

  s.kind = kind;
  s.db = *h;
  // Since the .def file is always merged onto the built-in defaults, which
  // never have a version or modnum, pretending these are always present is
  // a good enough approximation.
  s.kind_present = s.version_present = s.modnum_present = true;

  merge_state (s);
  }

static void
multicode_section (const char* secname) {
  unsigned int id = 2 + bininfo.extracode.size();

  if (bininfo.extracode.find (secname) == bininfo.extracode.end())
    bininfo.extracode[secname] = ResKey ("code", id);
  else
    einfo (E_FILE, "section `%s' duplicated", secname);
  }

static void
stack (unsigned long stack_size) {
  bininfo.stack_size = stack_size;
  }

static void
trap (unsigned int resid, unsigned int vector, const char* fname) {
  Datablock res (2);
  unsigned char* s = res.writable_contents();
  put_word (s, vector);
  add_resource (filename, ResKey ("trap", resid), res);

  if (fname) {

    }
  else {
    }
  }

static void
version_resource (unsigned long resid, const char* text) {
  // tver resources are null-terminated
  long size = strlen (text) + 1;
  Datablock block (size);
  memcpy (block.writable_contents(), text, size);
  add_resource (filename, // the filename of the .def file
		ResKey ("tver", resid), block);
  }

}

int
main (int argc, char** argv) {
  int c, longind;
  bool work_desired = true;

  char* output_fname = NULL;
  parameter_state opts;

  progname = argv[0];

  db_kind = DK_APPLICATION;
  init_database_header (&db);
  strncpy (db.type, "appl", 4);

  bininfo.stack_size = 4096;
  bininfo.force_rloc = false;
  bininfo.data_compression = 0;

  while ((c = getopt_long (argc, argv, shortopts, longopts, &longind)) != -1)
    switch (c) {
    case 'o':
      output_fname = optarg;
      break;

    case 'l':
      strncpy (opts.db.type, "GLib", 4);
      opts.kind = DK_GLIB;
      opts.kind_present = true;
      break;

    case 'L':
      strncpy (opts.db.type, "libr", 4);
      opts.kind = DK_SYSLIB;
      opts.kind_present = true;
      break;

    case 'a':
      db.appinfo = slurp_file_as_datablock (optarg);
      break;

    case 's':
      db.sortinfo = slurp_file_as_datablock (optarg);
      break;

    case 't':
      strncpy (opts.db.type, optarg, 4);
      break;

    case 'c':
      strncpy (opts.db.creator, optarg, 4);
      break;

    case 'n':
      strncpy (opts.db.name, optarg, 32);
      break;

    case 'm':
      opts.db.modnum = strtoul (optarg, NULL, 0);
      opts.modnum_present = true;
      break;

    case 'v':
      opts.db.version = strtoul (optarg, NULL, 0);
      opts.version_present = true;
      break;

    case 'z':
      bininfo.data_compression = strtoul (optarg, NULL, 0);
      break;

    case OPTION_READONLY:
      opts.db.readonly = true;
      break;

    case OPTION_APPINFO_DIRTY:
      opts.db.appinfo_dirty = true;
      break;

    case OPTION_BACKUP:
      opts.db.backup = true;
      break;

    case OPTION_OK_TO_INSTALL_NEWER:
      opts.db.ok_to_install_newer = true;
      break;

    case OPTION_RESET_AFTER_INSTALL:
      opts.db.reset_after_install = true;
      break;

    case OPTION_COPY_PREVENTION:
      opts.db.copy_prevention = true;
      break;

    case OPTION_STREAM:
      opts.db.stream = true;
      break;

    case OPTION_HIDDEN:
      opts.db.hidden = true;
      break;

    case OPTION_LAUNCHABLE_DATA:
      opts.db.launchable_data = true;
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
    return EXIT_SUCCESS;

  if (optind >= argc) {
    usage();
    return EXIT_FAILURE;
    }
  
  enum file_type first = file_type (argv[optind]);

  if (first == FT_PRC && !output_fname) {  // Old-style arguments
    if (argc - optind >= 3) {
      parameter_state s;
      output_fname = argv[optind++];
      strncpy (s.db.name, argv[optind++], 32);
      strncpy (s.db.creator, argv[optind++], 4);
      merge_state (s);

      if (opts.db.creator[0] != '\0' || opts.db.name[0] != '\0')
	einfo (E_NOFILE,
	       "`-o', `-n', '-c' options conflict with old-style arguments");
      }
    else {
      usage();
      nerrors++;
      }
    }
  else {  // New-style arguments
    if (!output_fname) {
      static char fname[FILENAME_MAX];
      strcpy (fname, argv[optind]);
      output_fname = basename_with_changed_extension (fname, ".prc");
      }
    }

  if (first == FT_DEF) {
    struct def_callbacks def_funcs = default_def_callbacks;
    def_funcs.db_header = db_header;
    def_funcs.multicode_section = multicode_section;
    def_funcs.stack = stack;
    def_funcs.trap = trap;
    def_funcs.version_resource = version_resource;
    read_def_file (argv[optind++], &def_funcs);

    if (opts.kind_present)
      einfo (E_NOFILE, "`-l' and `-L' options conflict with definition file");
    }

  merge_state (opts);

  if (nerrors)
    return EXIT_FAILURE;

  switch (db_kind) {
  case DK_APPLICATION:
    bininfo.maincode = ResKey ("code", 1);
    bininfo.emit_appl_extras = true;
    bininfo.emit_data = true;
    break;

  case DK_GLIB:
    bininfo.maincode = ResKey ("GLib", 0);
    bininfo.emit_appl_extras = false;
    bininfo.emit_data = true;
    break;

  case DK_SYSLIB:
    bininfo.maincode = ResKey ("libr", 0);
    bininfo.emit_appl_extras = false;
    bininfo.emit_data = false;
    break;

  case DK_HACK:
    bininfo.maincode = ResKey ("code", 1000);
    bininfo.emit_appl_extras = false;
    bininfo.emit_data = false;
    break;

  case DK_GENERIC:
    bininfo.maincode = ResKey ("code", 0);
    bininfo.emit_appl_extras = false;
    bininfo.emit_data = false;
    break;
    }

  for (int i = optind; i < argc; i++)
    switch (file_type (argv[i])) {
    case FT_DEF:
      einfo (E_NOFILE,
	     (first == FT_DEF)? "only one definition file may be used"
			      : "the definition file must come first");
      break;

    case FT_RAW: {
      char buffer[FILENAME_MAX];
      strcpy (buffer, argv[i]);
      char *key = basename_with_changed_extension (buffer, NULL);
      char *dot = strchr (key, '.');
      /* A dot in the first four characters might just possibly be a very
	 strange resource type.  Any beyond there are extensions, which
	 we'll remove.  */
      if (dot && dot-key >= 4)  *dot = '\0';
      if (strlen (key) < 8) {
	while (strlen (key) < 4)  strcat (key, "x");
	while (strlen (key) < 8)  strcat (key, "0");
	filename = argv[i];
	einfo (E_FILE | E_WARNING,
	       "raw filename doesn't start with `typeNNNN'; treated as `%s'",
	       key);
	}
      else
	key[8] = '\0';  /* Ensure strtoul() doesn't get any extra digits.  */

      add_resource (argv[i], ResKey (key, strtoul (&key[4], NULL, 16)),
		    slurp_file_as_datablock (argv[i]));
      }
      break;

    case FT_BFD: {
      ResourceDatabase bfd_db = process_binary_file (argv[i], bininfo);
      for (ResourceDatabase::const_iterator it = bfd_db.begin();
	   it != bfd_db.end();
	   ++it)
	add_resource (argv[i], (*it).first, (*it).second);
      }
      break;

    case FT_PRC:
      // @@@ implement this later
      einfo (E_NOFILE | E_WARNING,
	     "`%s' ignored: PRC reading not yet implemented", argv[i]);
      break;
      }

  if (nerrors == 0) {
    if (db.name[0] == '\0')
      einfo (E_NOFILE | E_WARNING,
	     "creating `%s' without a name", output_fname);
    if (db.creator[0] == '\0')
      einfo (E_NOFILE | E_WARNING,
	     "creating `%s' without a creator id", output_fname);

    FILE* f = fopen (output_fname, "wb");
    if (f) {
      time_t now = time (NULL);
      struct tm* now_tm = localtime (&now);
      db.created = db.modified = *now_tm;
      if (db.write (f))
	fclose (f);
      else {
	einfo (E_NOFILE | E_PERROR, "error writing to `%s'", output_fname);
	fclose (f);
	remove (output_fname);
	}
      }
    else
      einfo (E_NOFILE | E_PERROR, "can't write to `%s'", output_fname);
    }

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
