/* dirutils.c: various filesystem-related utilities.

   Copyright 2002 by John Marshall.
   <jmarshall@acm.org>

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "libiberty.h"

#include "utils.h"

static int
vis_dir (const char *pathformat, va_list *args) {
  char path[FILENAME_MAX];
  struct stat st;

  vsprintf (path, pathformat, *args);
  return stat (path, &st) == 0 && S_ISDIR (st.st_mode);
  }

int
is_dir (const char *pathformat, ...) {
  va_list args;
  int rc;

  va_start (args, pathformat);
  rc = vis_dir (pathformat, &args);
  va_end (args);
  return rc;
  }

int
is_dir_dirent (struct dirent *dent, const char *pathformat, ...) {
  va_list args;
  int rc;

#if defined _DIRENT_HAVE_D_TYPE && defined DT_DIR
  if (dent && dent->d_type == DT_DIR)
    return 1;
#endif

  va_start (args, pathformat);
  rc = vis_dir (pathformat, &args);
  va_end (args);
  return rc;
  }


struct directory_node {
  struct directory_node *next;
  char path[FILENAME_MAX];
  };

struct directory_tree {
  int flags;
  DIR *curdir;
  char curname[FILENAME_MAX];
  struct directory_node *dirstack;
  };

static void
push (TREE *tree, const char *path) {
  struct directory_node *n = xmalloc (sizeof (struct directory_node));

  strcpy (n->path, path);
  n->next = tree->dirstack;
  tree->dirstack = n;
  }

static void
pop (TREE *tree) {
  struct directory_node *next = tree->dirstack->next;
  free (tree->dirstack);
  tree->dirstack = next;
  }

TREE *
opentree (int flags, const char *path_format, ...) {
  char path[FILENAME_MAX];
  va_list args;
  TREE *tree = xmalloc (sizeof (struct directory_tree));

  va_start (args, path_format);
  vsprintf (path, path_format, args);
  va_end (args);

  tree->flags = flags;
  tree->curdir = NULL;
  tree->dirstack = NULL;
  push (tree, path);

  return tree;
  }

const char *
readtree (TREE *tree) {
  struct dirent *entry;
  
  while (1)
    if (tree->curdir && (entry = readdir (tree->curdir)) != NULL) {
      static char entryname[FILENAME_MAX];

      sprintf (entryname, "%s/%s", tree->curname, entry->d_name);
      if (is_dir_dirent (entry, "%s", entryname)) {
	if (strcmp (entry->d_name, ".") != 0
	    && strcmp (entry->d_name, "..") != 0)
	  push (tree, entryname);
	}
      else {
	if (tree->flags & FILES)
	  return entryname;
	}
      }
    else if (tree->dirstack) {
      if (tree->curdir)
	closedir (tree->curdir);

      strcpy (tree->curname, tree->dirstack->path);
      pop (tree);

      tree->curdir = opendir (tree->curname);
      if (tree->curdir && (tree->flags & DIRS))
	return tree->curname;
      }
    else
      return NULL;
  }

void
closetree (TREE *tree) {
  if (tree->curdir)
    closedir (tree->curdir);
  while (tree->dirstack)
    pop (tree);
  free (tree);
  }
