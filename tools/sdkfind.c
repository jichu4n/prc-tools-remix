/* sdkfind.c: generate -I options etc for all subdirectories of an SDK.

   Copyright (c) 2001 Palm, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <unistd.h>

#include "libiberty.h"

#include "utils.h"


const char *progname = "sdkfind";
const char *progversion = "1.0";


char **args = NULL, **arglim = NULL, **argmax = NULL;
struct string_store *store;

void
add_space (int n) {
  ptrdiff_t nlim = (args)? arglim - args : 0;
  ptrdiff_t nmax = (args)? argmax - args : 0;
  nmax += n;
  args = xrealloc (args, nmax * sizeof (const char *));
  arglim = args + nlim;
  argmax = args + nmax;
  }

void
add (char *s) {
  if (arglim == argmax)
    add_space (32);
  *arglim++ = s;
  }


void
add_isystem (const char *path) {
  add ("-isystem");
  add (insert_string (store, path));
  }

void
add_L (const char *path) {
  char buf[2 + FILENAME_MAX];
  strcpy (buf, "-L");
  strcat (buf, path);
  add (insert_string (store, buf));
  }


int
is_dir (const char *path) {
  struct stat st;
  return stat (path, &st) == 0 && S_ISDIR (st.st_mode);
  }

int
add_tree (void (*addf) (const char *), const char *dirname) {
  DIR *dir;

  if (is_dir (dirname) && (dir = opendir (dirname)) != NULL) {
    struct dirent *dirent;

    addf (dirname);

    while ((dirent = readdir (dir)) != NULL)
      if (dirent->d_name[0] != '.') {
	char fname[FILENAME_MAX];
	sprintf (fname, "%s/%s", dirname, dirent->d_name);
	add_tree (addf, fname);
	}

    closedir (dir);
    return 1;
    }
  else
    return 0;
  }

void
add_trees (const char *sdk_version) {
  char sdk[FILENAME_MAX];
  char path[FILENAME_MAX];

  int broken_sdk = 0;

  sprintf (sdk, "sdk%s%s",
	   (*sdk_version && *sdk_version != '-')? "-" : "", sdk_version);

  sprintf (path, "%s/include", PALMDEV_PREFIX);
  add_tree (add_isystem, path);

  sprintf (path, "%s/%s/include", PALMDEV_PREFIX, sdk);
  if (!add_tree (add_isystem, path))
    broken_sdk = 1;

  sprintf (path, "%s/lib/%s", PALMDEV_PREFIX, TARGET);
  add_tree (add_L, path);

  sprintf (path, "%s/%s/lib/%s", PALMDEV_PREFIX, sdk, TARGET);
  /* We would update BROKEN_SDK here too, except that pre-3.5 SDKs didn't
     have lib directories.  */
  add_tree (add_L, path);

  sprintf (path, "%s/%s", PALMDEV_PREFIX, sdk);
  if (!is_dir (path))
    einfo (E_NOFILE|E_WARNING, "%s does not exist", path);
  else if (broken_sdk)
    einfo (E_NOFILE|E_WARNING, "%s does not contain an sdk", path);
  }


int
prefix_strcmp (const char *s, const char *prefix) {
  return strncmp (s, prefix, strlen (prefix));
  }

void
dump (FILE *f, char **ap) {
  while (*ap)
    fprintf (f, " %s", *ap++);
  putc ('\n', f);
  }


int
main (int argc, char **argv) {
  int verbose_seen, print_search_dirs_seen, palmos_seen, retcode, i;
  const char *subcmd;

  xmalloc_set_program_name (progname);
  retcode = EXIT_SUCCESS;

  subcmd = basename (argv[0]);
  if (prefix_strcmp (subcmd, TARGET"-") == 0)
    subcmd += strlen (TARGET"-");
  else if (prefix_strcmp (subcmd, TARGET_ALIAS"-") == 0)
    subcmd += strlen (TARGET_ALIAS"-");

  add_space (argc);
  store = new_string_store ();

  verbose_seen = print_search_dirs_seen = palmos_seen = 0;

  for (i = 0; i < argc; i++)
    if (prefix_strcmp (argv[i], "-palmos") == 0) {
      add_trees (argv[i] + strlen ("-palmos"));
      palmos_seen = 1;
      }
    else if (strcmp (argv[i], "-nopalmos") == 0)
      palmos_seen = 1;
    else {
      if (strcmp (argv[i], "-v") == 0)
	verbose_seen = 1;
      else if (strcmp (argv[i], "-print-search-dirs") == 0)
	print_search_dirs_seen = 1;
      add (argv[i]);
      }

  if (!palmos_seen && !print_search_dirs_seen)
    add_trees ("");

  add (NULL);

  if (print_search_dirs_seen)
    printf ("palmdev: %s/\n", PALMDEV_PREFIX);

  if (strcmp (subcmd, progname) == 0)
    dump (stdout, args + 1);
  else {
    char *errmsg_fmt, *errmsg_arg;
    char full_subcmd[FILENAME_MAX];
    int pid, status;

    sprintf (full_subcmd, "%s/%s/real-bin/%s%s",
	     EXEC_PREFIX, TARGET_ALIAS, subcmd, EXEEXT);

    if (verbose_seen) {
      fprintf (stderr, " [%s]", full_subcmd);
      dump (stderr, args);
      }

    pid = pexecute (full_subcmd, args, progname,
		    NULL, &errmsg_fmt, &errmsg_arg, PEXECUTE_ONE);

    if (pid >= 0) {
      pwait (pid, &status, 0);
      if (WIFSIGNALED (status))
	retcode = EXIT_FAILURE;
      else if (WIFEXITED (status))
	retcode = WEXITSTATUS (status);
      }
    else
      einfo (E_NOFILE|E_PERROR, errmsg_fmt, errmsg_arg);
    }

  free_string_store (store);
  free (args);

  if (nerrors > 0)
    retcode = EXIT_FAILURE;

  return retcode;
  }
