/* trapfilt.c: add Palm OS trap names to disassembly listings.

   Copyright 2002, 2003, 2004 John Marshall.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "libiberty.h"
#include "getopt.h"

#include "utils.h"

static char *traptext;
static const char **trapname;
static unsigned int minvector, maxvector;

void
load_trapnumbers () {
  static const char fname[] = DATA_PREFIX"/trapnumbers";

  traptext = slurp_text_file (fname);
  if (traptext) {
    unsigned int i;

    /* Skip the first line, which could contain spurious '*' characters.  */
    char *s = strchr (traptext, '\n');

    s = strchr (s, '*');
    sscanf (s, "*%*d%x%x", &minvector, &maxvector);

    trapname = xmalloc ((maxvector - minvector + 1) * sizeof (const char *));
    for (i = minvector; i <= maxvector; i++)  trapname[i - minvector] = NULL;

    /* Skip to the end of the line, so we're aimed at the first trap line.  */
    s = strchr (s, '\n');

    for (s = strtok (s, " \t\n"); s; s = strtok (NULL, " \t\n"))
      trapname[strtoul (s, NULL, 0) - minvector] = strtok (NULL, " \t\n");
    }
  else {
    error ("can't open '%s': @P", fname);
#ifdef ENOENT
    if (errno == ENOENT)
      error ("(rerun palmdev-prep to create the trapnumbers data file)");
#endif
    minvector = 1;
    maxvector = 0;
    }
  }

const char *
lookup (unsigned int v) {
  return (minvector <= v && v <= maxvector)? trapname[v - minvector] : NULL;
  }

void
free_trapnumbers () {
  free (traptext);
  free (trapname);
  }


static int show_help, show_version;

static const char shortopts[] = "q";

static struct option longopts[] = {
  { "quiet", no_argument, NULL, 'q' },
  { "silent", no_argument, NULL, 'q' },

  { "help", no_argument, &show_help, 1 },
  { "version", no_argument, &show_version, 1 },
  { NULL, no_argument, NULL, 0 }
  };

int
main (int argc, char **argv) {
  int verbose = 1;
  int c;

  set_progname (argv[0]);

  while ((c = getopt_long (argc, argv, shortopts, longopts, NULL)) >= 0)
    switch (c) {
    case 'q':
      verbose = 0;
      break;

    case 0:
      /* This indicates an automatically handled long option: do nothing.  */
      break;

    default:
      nerrors++;
      show_help = 1;
      break;
      }

  if (show_version || show_help) {
    if (show_version)
      print_version ("trapfilt", "Jg");

    if (show_help)
      printf ("Usage: %s [-q | --quiet | --silent] [vector...]\n", progname);
    }
  else {
    load_trapnumbers ();

    if (optind == argc) {
      char line[2048];

      while (fgets (line, sizeof line, stdin)) {
	unsigned int hex, oct;
	const char *name;
	char *nl = strchr (line, '\n');
	if (nl)  *nl = '\0';
	if (sscanf (line, "%*x:%x%o", &hex, &oct) == 2 && hex == oct
	    && (name = lookup (hex)) != NULL)
	  printf ("%s <%s>\n", line, name);
	else
	  puts (line);
	}
      }
    else
      for (; optind < argc; optind++) {
	/* If it's unparsable, strtoul returns 0, which will not be found.  */
	const char *name = lookup (strtoul (argv[optind], NULL, 0));
	if (verbose) {
	  printf ("%s", argv[optind]);
	  if (name)  printf (" <%s>", name);
	  printf ("\n");
	  }
	else {
	  if (name)
	    printf ("%s\n", name);
	  else
	    error ("no trap name found for '%s'", argv[optind]);
	  }
	}

    free_trapnumbers ();
    }

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
