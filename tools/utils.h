/* utils.h: various utilities.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.
 */

#ifndef _UTILS_H_
#define _UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

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

void ewhere (const char *format, ...);
void einfo (int type, const char *format, ...);


extern int propt_tab;
void propt (const char *optname, const char *meaning);


enum file_type {
  FT_RAW,	/* .grc or .bin */
  FT_BFD,	/* executable (no extension) */
  FT_PRC,	/* .prc */
  FT_DEF	/* .def */
  };

enum file_type file_type (const char *fname);

void *slurp_file (const char *fname, const char *mode, long *sizep);


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

const char * standard_db_type (enum database_kind kind);


struct string_store;

struct string_store *new_string_store ();
char *insert_string (struct string_store *store, const char *s);
void free_string_store (struct string_store *store);

#ifdef __cplusplus
}
#endif

#endif
