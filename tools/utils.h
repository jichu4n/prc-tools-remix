/* utils.h: various utilities.

   Copyright (c) 1998, 1999, 2001 by John Marshall.
   <jmarshall@acm.org>

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Use "set_progname (argv[0])" near the start of main().  The name given
   is available thereafter via the global PROGNAME.  */

void set_progname (const char *progname0);
extern const char *progname;

extern int nerrors, nwarnings;

#ifdef __GNUC__
#define PRINTF_FUNC(fmt, first)  __attribute__ ((format (printf, fmt, first)))
#else
#define PRINTF_FUNC(fmt, first)
#endif

/* These functions increment either NERRORS or NWARNINGS respectively, and
   print a formatted error message to STDERR.  An initial part of FORMAT
   enclosed in "["..."]" denotes a filename and other location information
   and is formatted specially.  If there is no "["..."]" part, PROGNAME is
   printed instead.  If FORMAT ends with "@P", those two characters are
   replaced by the output of perror().  Examples:

	error ("my bad")
		<progname>: my bad\n

	warning ("[%s:%d] strange specifiers", "foo.c", 15)
		foo.c:15: warning: strange specifiers\n

	error ("[%s] can't open %s: @P", "foo.c", "blah.h")
		foo.c: can't open blah.h: <perror("") output>\n  */

void error   (const char *format, ...) PRINTF_FUNC (1, 2);
void warning (const char *format, ...) PRINTF_FUNC (1, 2);

#undef PRINTF_FUNC

extern int propt_tab;
void propt (const char *optname, const char *meaning);

/* Print a version line (using the general prc-tools version number from
   prc-tools.spec) and various copyright messages.  CANONICAL_PROGNAME may
   differ from PROGNAME -- see the GNU coding standard, standards.info.

   FLAGS contains mainly letters [A-Za-z] indicating copyright holders to be
   printed.  Upper case letters are printed with "Copyright", lower case with
   "Portions copyright".  See the function for valid letters.

   The character "=" in FLAGS means print "(TARGET_ALIAS prc-tools)" instead
   of plain "(prc-tools)" on the version line.  */

void print_version (const char *canonical_progname, const char *flags);


/* If NEWEXT is non-NULL, strips off any extension (a final '.' and all
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
