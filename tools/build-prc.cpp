/* build-prc.cpp: build a .prc from a pile of files.

   Copyright (c) 1998, 1999 Palm Computing, Inc. or its subsidiaries.
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

static const char* version = "2.0";

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

static ResourceDatabase db;
static ResourceProvenance prov;

bool version_flag_present = false;
bool modnum_flag_present = false;


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


static struct binary_file_info bininfo;

static void
set_bininfo (database_kind kind) {
  switch (kind) {
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
  }

// I'm not sure I really understand how to do this, but we'll try:
extern "C" {

static void
db_header (database_kind kind, const struct database_header* h) {
  set_bininfo (kind);

  // Update header fields from the .def file if they weren't given values
  // on the command line.  Only version and modnum need X_present indicators,
  // because only they are likely to be explicitly set to zero.

  if (db.name[0] == '\0')  strncpy (db.name, h->name, 32);
  if (db.type[0] == '\0')  strncpy (db.type, h->type, 4);
  if (db.creator[0] == '\0')  strncpy (db.creator, h->creator, 4);

  db.readonly |= h->readonly;
  db.appinfo_dirty |= h->appinfo_dirty;
  db.backup |= h->backup;
  db.ok_to_install_newer |= h->ok_to_install_newer;
  db.reset_after_install |= h->reset_after_install;
  db.copy_prevention |= h->copy_prevention;
  db.stream |= h->stream;
  db.hidden |= h->hidden;
  db.launchable_data |= h->launchable_data;

  if (!version_flag_present)  db.version = h->version;
  if (!modnum_flag_present)  db.modnum = h->modnum;

  if (!palmostime_of_tm (&db.created))  db.created = h->created;
  if (!palmostime_of_tm (&db.modified))  db.modified = h->modified;
  if (!palmostime_of_tm (&db.backedup))  db.backedup = h->backedup;
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

  progname = argv[0];

  init_database_header (&db);

  database_kind oldstyle_db_kind = DK_APPLICATION;

  bininfo.stack_size = 4096;
  bininfo.force_rloc = false;
  bininfo.data_compression = 0;

  while ((c = getopt_long (argc, argv, shortopts, longopts, &longind)) != -1)
    switch (c) {
    case 'o':
      output_fname = optarg;
      break;

    case 'l':
      strncpy (db.type, "GLib", 4);
      oldstyle_db_kind = DK_GLIB;
      break;

    case 'L':
      strncpy (db.type, "libr", 4);
      oldstyle_db_kind = DK_SYSLIB;
      break;

    case 'a':
      db.appinfo = slurp_file_as_datablock (optarg);
      break;

    case 's':
      db.sortinfo = slurp_file_as_datablock (optarg);
      break;

    case 't':
      strncpy (db.type, optarg, 4);
      break;

    case 'c':
      strncpy (db.creator, optarg, 4);
      break;

    case 'n':
      strncpy (db.name, optarg, 32);
      break;

    case 'm':
      db.modnum = strtoul (optarg, NULL, 0);
      modnum_flag_present = true;
      break;

    case 'v':
      db.version = strtoul (optarg, NULL, 0);
      version_flag_present = true;
      break;

    case 'z':
      bininfo.data_compression = strtoul (optarg, NULL, 0);
      break;

    case OPTION_READONLY:
      db.readonly = true;
      break;

    case OPTION_APPINFO_DIRTY:
      db.appinfo_dirty = true;
      break;

    case OPTION_BACKUP:
      db.backup = true;
      break;

    case OPTION_OK_TO_INSTALL_NEWER:
      db.ok_to_install_newer = true;
      break;

    case OPTION_RESET_AFTER_INSTALL:
      db.reset_after_install = true;
      break;

    case OPTION_COPY_PREVENTION:
      db.copy_prevention = true;
      break;

    case OPTION_STREAM:
      db.stream = true;
      break;

    case OPTION_HIDDEN:
      db.hidden = true;
      break;

    case OPTION_LAUNCHABLE_DATA:
      db.launchable_data = true;
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

  bool deffile_present = argc > optind && file_type (argv[optind]) == FT_DEF;

  for (int i = optind+1; i < argc; i++)
      if (file_type (argv[i]) == FT_DEF)
	if (deffile_present)
	  einfo (E_NOFILE, "only one .def file can be used");
	else
	  einfo (E_NOFILE, "the .def file must come first");

  if (output_fname || deffile_present) {  // New-style arguments
    if (deffile_present && 0 /* -l or -L */)
      einfo (E_NOFILE, "`-l' and `-L' options conflict with definition file");

    if (!output_fname) {
      static char fname[FILENAME_MAX];
      strcpy (fname, argv[optind]);
      output_fname = basename_with_changed_extension (fname, ".prc");
      }
    }
  else {  // Old-style arguments
    if (output_fname || db.creator[0] != '\0' || db.name[0] != '\0')
      einfo (E_NOFILE,
	     "`-o', `-n', '-c' options conflict with old-style arguments");
    if (argc - optind >= 3) {
      output_fname = argv[optind++];

      strncpy (db.name, argv[optind++], 32);
      if (db.type[0] == '\0')  strncpy (db.type, "appl", 4);
      strncpy (db.creator, argv[optind++], 4);

      set_bininfo (oldstyle_db_kind);
      }
    else
      nerrors++;
    }

  if (nerrors) {
    usage();
    return EXIT_FAILURE;
    }

  struct def_callbacks def_funcs;
  def_funcs = default_def_callbacks;
  def_funcs.db_header = db_header;
  def_funcs.multicode_section = multicode_section;
  def_funcs.stack = stack;
  def_funcs.trap = trap;
  def_funcs.version_resource = version_resource;

  for (int i = optind; i < argc; i++)
    switch (file_type (argv[i])) {
    case FT_DEF:
      read_def_file (argv[i], &def_funcs);
      break;

    case FT_RAW:
      add_resource (argv[i], ResKey (argv[i], strtoul (&argv[i][4], NULL, 16)),
		    slurp_file_as_datablock (argv[i]));
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
      break;
      }

  if (nerrors == 0) {
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
