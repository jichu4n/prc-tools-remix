/* multigen.c: generate a multiple section stub file and linker script from
   a .def file.

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

const char progversion[] = "1.2";

void
usage() {
  printf ("\
Usage: %s [options] deffile\n\
Creates assembly stub file '<base>.s' and linker script '<base>.ld' from\n\
multiple code clause in <deffile>; <base> is '<deffile>-sections' by default.\n\
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


struct section_entry {
  const char *name;
  struct section_entry *next;
  };

static const char *deffname;
static struct section_entry *first_section_entry = NULL;
static struct section_entry **next_section_entry_ptr = &first_section_entry;

static void
section (const char *secname) {
  struct section_entry *entry = malloc (sizeof (struct section_entry));
  entry->name = secname;
  entry->next = NULL;
  *next_section_entry_ptr = entry;
  next_section_entry_ptr = &entry->next;
  }


static int
filter (FILE *f, const char *key) {
  struct section_entry *e;

  if (strcmp (key, "@deffile@") == 0)
    fprintf (f, "%s", deffname);
  else if (strcmp (key, "@mem-entries@") == 0)
    for (e = first_section_entry; e; e = e->next)
      fprintf (f, "\t%sres : ORIGIN = 0x0, LENGTH = 32768\n", e->name);
  else if (strcmp (key, "@sec-entries@") == 0)
    for (e = first_section_entry; e; e = e->next)
      fprintf (f, "\t%s : { *(%s) } > %sres\n", e->name, e->name, e->name);
  else if (strcmp (key, "@variables@") == 0) {
    struct section_entry main_section;

    main_section.name = "";
    main_section.next = first_section_entry;

    for (e = &main_section; e; e = e->next) {
      fprintf (f, "\t.globl\t__text__%s\n", e->name);
      fprintf (f, "\t.lcomm\t__text__%s,4\n", e->name);
      }
    }
  else
    return 0;

  return 1;
  }


static const char *const stub_text[] = {
#include "multicode-s.str"
  };

static const char *const linker_text[] = {
#include "multicode-ld.str"
  };


int
main (int argc, char **argv) {
  int c, longind;
  int work_desired = 1;
  char *outfbase = NULL;

  struct def_callbacks funcs = default_def_callbacks;
  funcs.multicode_section = section;

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

    if (first_section_entry == NULL)
      warning ("[%s] no multiple code sections specified", deffname);

    if (outfbase) {
      strcpy (buffer, outfbase);
      outfname = buffer;
      }
    else {
      strcpy (buffer, deffname);
      outfname = basename_with_changed_extension (buffer, "-sections");
      }

    eos = strchr (outfname, '\0');

    strcpy (eos, ".s");
    generate_file_from_template (outfname, stub_text, filter);

    strcpy (eos, ".ld");
    generate_file_from_template (outfname, linker_text, filter);
    }

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
