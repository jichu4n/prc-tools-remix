/* trapfilt.c: add systrap names to disassembly listings.

   Copyright (c) 2001 Palm, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "libiberty.h"
#include "utils.h"

const char **trapname;
unsigned int mintrap, maxtrap;

void
load_systraps (FILE *f, struct string_store *store) {
  char line[512];

  while (fgets (line, sizeof line, f))
    if (line[0] == '-') {
      unsigned int num;
      char name[512];
      sscanf (line, "- %s 0x%x", name, &num);
      if (mintrap <= num && num <= maxtrap)
	trapname[num - mintrap] = insert_string (store, name);
      }
    else if (line[0] == '*') {
      int ntraps, i;
      sscanf (line, "* %d 0x%x 0x%x", &ntraps, &mintrap, &maxtrap);
      trapname = xmalloc (ntraps * sizeof (const char *));
      for (i = 0; i < ntraps; i++)
	trapname[i] = NULL;
      }
  }

void
filter (FILE *f) {
  char line[2048];
  unsigned long addr;
  unsigned int hex, oct;

  while (fgets (line, sizeof line, f))
    if (sscanf (line, "%lx:%x%o", &addr, &hex, &oct) == 3
	&& hex == oct && mintrap <= hex && hex <= maxtrap) {
      char *nl = strchr (line, '\n');
      if (nl)  *nl = '\0';
      printf ("%s <%s>\n", line, trapname[hex - mintrap]);
      }
    else
      fputs (line, stdout);
  }

int
main (int argc, char **argv) {
  struct string_store *name_store = new_string_store ();
  const char systrap_fname[] = PALMDEV_PREFIX"/lib/trapnumbers";
  FILE *f;
  int i;

  set_progname (argv[0]);

  if ((f = fopen (systrap_fname, "r")) != NULL) {
    load_systraps (f, name_store);
    fclose (f);
    }
  else {
    error ("%s: @P", systrap_fname);
    mintrap = maxtrap = 0;
    trapname = NULL;
    }

  if (argc == 1)
    filter (stdin);
  else
    for (i = 1; i < argc; i++) {
      FILE *f = fopen (argv[i], "r");
      if (f) {
	filter (f);
	fclose (f);
	}
      else
	error ("%s: @P", argv[i]);
      }

  free (trapname);
  free_string_store (name_store);

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
