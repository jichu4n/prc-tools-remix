/* stubgen.c: generate a library dispatch stub file from a .def file.

   Copyright (c) 1999-2001 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"
#include "utils.h"
#include "def.h"
#include "pfdheader.h"

const char progversion[] = "1.2";

void
usage() {
  printf ("\
Usage: %s [options] deffile\n\
Creates GLib support files ('<base>-jumps.s' and '<base>-stubs.c') or\n\
a SysLib support file ('<base>-dispatch.s') from an export clause in\n\
<deffile>; <base> is '<deffile>' by default.\n\
Options:\n", progname);
  propt ("-b FILE, --base FILE", "Set output filename base prefix");
  }

enum {
  OPTION_HELP = 150,
  OPTION_VERSION
  };

static const char shortopts[] = "b:";

struct option longopts[] = {
  { "base", required_argument, NULL, 'b' },
  { "help", no_argument, NULL, OPTION_HELP },
  { "version", no_argument, NULL, OPTION_VERSION },
  { NULL, no_argument, NULL, 0 }
  };

struct function_entry {
  const char *name;
  struct function_entry *next;
  };

static const char *deffname;
static enum database_kind db_kind;
static struct database_header db_header;
static struct function_entry *first_function_entry = NULL;
static struct function_entry **next_function_entry_ptr = &first_function_entry;

static void
db_header_func (enum database_kind kind, const struct database_header *h) {
  db_kind = kind;
  db_header = *h;
  }

static void
export_function (const char *funcname) {
  struct function_entry *entry = malloc (sizeof (struct function_entry));
  entry->name = funcname;
  entry->next = NULL;
  *next_function_entry_ptr = entry;
  next_function_entry_ptr = &entry->next;
  }


static int
filter (FILE *f, const char *key) {
  struct function_entry *entry;
  int n;

  if (strcmp (key, "@deffile@") == 0)
    fprintf (f, "%s", deffname);
  else if (strcmp (key, "@libname@") == 0)
    fprintf (f, "%.*s", (int) sizeof (db_header.name), db_header.name);
  else if (strcmp (key, "@crid@") == 0)
    fprintf (f, "%.*s", (int) sizeof (db_header.creator), db_header.creator);
  else if (strcmp (key, "@function-offsets@") == 0)
    for (entry = first_function_entry; entry; entry = entry->next)
      fprintf (f, "\tdc.w\t%s-jmptable\n", entry->name);
  else if (strcmp (key, "@function-jumps@") == 0)
    for (entry = first_function_entry, n = 1; entry; entry = entry->next, n++) {
      fprintf (f, "\t.global %s\n%s:\n", entry->name, entry->name);
      fprintf (f, "\tmove.l\t#%d,%%%%d0\n", n);
      fprintf (f, "\tbraw\tdispatch\n");
      }
  else
    return 0;

  return 1;
  }


static const char *const glib_jumptable_text[] = {
#include "glib-jumps-s.str"
  };

static const char *const glib_stub_text[] = {
#include "glib-stubs-c.str"
  };

static const char *const syslib_dispatch_text[] = {
#include "syslib-dispatch-s.str"
  };


int
main (int argc, char **argv) {
  int c, longind;
  int work_desired = 1;
  char *outfbase = NULL;

  struct def_callbacks funcs = default_def_callbacks;
  funcs.db_header = db_header_func;
  funcs.export_function = export_function;

  progname = argv[0];

  while ((c = getopt_long (argc, argv, shortopts, longopts, &longind)) != -1)
    switch (c) {
    case 'b':
      outfbase = optarg;
      break;

    case OPTION_HELP:
      usage ();
      work_desired = 0;
      break;

    case OPTION_VERSION:
      printf ("%s version %s\n", progname, progversion);
      work_desired = 0;
      break;
      }

  if (!work_desired)
    return 0;

  if (optind + 1 != argc) {
    usage ();
    return 0;
    }

  deffname = argv[optind];

  read_def_file (deffname, &funcs);

  if (nerrors == 0) {
    char buffer[FILENAME_MAX];
    char *outfname, *eos;

    if (first_function_entry == NULL)
      warning ("[%s] no functions exported", deffname);

    if (outfbase) {
      strcpy (buffer, outfbase);
      outfname = buffer;
      }
    else {
      strcpy (buffer, deffname);
      outfname = basename_with_changed_extension (buffer, "");
      }

    eos = strchr (outfname, '\0');

    switch (db_kind) {
    case DK_SYSLIB:
      strcpy (eos, "-dispatch.s");
      generate_file_from_template (outfname, syslib_dispatch_text, filter);
      break;

    case DK_GLIB:
      strcpy (eos, "-jumps.s");
      generate_file_from_template (outfname, glib_jumptable_text, filter);
      strcpy (eos, "-stubs.c");
      generate_file_from_template (outfname, glib_stub_text, filter);
      break;

    default:
      error ("[%s] project is not a shared library", deffname);
      break;
      }
    }

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
