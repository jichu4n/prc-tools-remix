/* utils.h: various utilities.

   Copyright (c) 1998, 1999 by John Marshall.
   <jmarshall@acm.org>

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* This one must be defined by the main program.  */
extern const char *progversion;

extern const char *progname, *filename;
extern int lineno;

extern int nerrors, nwarnings;

enum {
  /* Use exactly one of these: */
  E_NOFILE = 0,		/* "progname: " */
  E_FILE = 1,		/* "filename: " */
  E_FILELINE = 2,	/* "filename:lineno: " */
  E_FILEWHERE = 3,	/* "filename:%s: " -- set %s with ewhere() */

  /* Add in any of these with `|': */
  E_WARNING = 0x10,
  E_PERROR = 0x20
  };

#ifdef __GNUC__
#define PRINTF_FUNC(fmt, first)  __attribute__ ((format (printf, fmt, first)))
#else
#define PRINTF_FUNC(fmt, first)
#endif

void ewhere (const char *format, ...) PRINTF_FUNC (1, 2);
void einfo (int type, const char *format, ...) PRINTF_FUNC (2, 3);

#undef PRINTF_FUNC

extern int propt_tab;
void propt (const char *optname, const char *meaning);


enum file_type {
  FT_UNKNOWN,
  FT_RAW,	/* .grc or .bin */
  FT_BFD,	/* executable (no extension) */
  FT_PRC,	/* .prc or .ro */
  FT_DEF	/* .def */
  };

enum file_type file_type (const char *fname);

/* If NEWEXT is non-NULL, strips off any extension (a final `.' and all
   following characters) from FNAME and appends NEWEXT.  Returns a pointer to
   the start of the filename part (i.e., without any directories) of FNAME.  */
char *basename_with_changed_extension (char *fname, const char *newext);

void *slurp_file (const char *fname, const char *mode, long *sizep);

int copy_file (const char *outfname, const char *infname, const char *mode);

void generate_file_from_template (const char *fname, const char *const *tmpl,
				  int (*filter)(FILE *f, const char *key));

void chomp (char *s);


#ifndef _GNU_SOURCE
void *memmem (const void *buf, size_t buflen, const void *key, size_t keylen);
#endif


enum database_kind {
  DK_APPLICATION,
  DK_GLIB,
  DK_SYSLIB,
  DK_HACK,
  DK_GENERIC
  };

char *standard_db_type (enum database_kind kind);


struct string_store;

struct string_store *new_string_store ();
char *insert_string (struct string_store *store, const char *s);
void free_string_store (struct string_store *store);

#ifdef __cplusplus
}
#endif

#endif
