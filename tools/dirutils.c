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

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

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


static void
for_each_subdir_aux (int (*process) (const char *, const char *),
		     const char *path, const char *base) {
  DIR *dir = opendir (path);
  if (dir == NULL)
    return;

  if (base == NULL || process (path, base)) {
    struct dirent *e;
    while ((e = readdir (dir)) != NULL)
      if (e->d_name[0] != '.' && is_dir_dirent (e, "%s/%s", path, e->d_name)) {
	char fullname[FILENAME_MAX];
	sprintf (fullname, "%s/%s", path, e->d_name);
	for_each_subdir_aux (process, fullname, e->d_name);
	}
    }

  closedir (dir);
  }

void
for_each_subdir (int (*process) (const char *, const char *),
		 int process_top, const char *pathformat, ...) {
  char path[FILENAME_MAX];
  const char *base;
  va_list args;

  va_start (args, pathformat);
  vsprintf (path, pathformat, args);
  va_end (args);

  if (process_top) {
    const char *sep = strrchr (path, '/');
    base = sep? sep + 1 : path;
    }
  else
    base = NULL;

  for_each_subdir_aux (process, path, base);
  }
