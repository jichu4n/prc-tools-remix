/* utils.c: various utilities.

   Copyright (c) 1998, 1999 by John Marshall.
   <jmarshall@acm.org>

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "libiberty.h"

#include "utils.h"

const char *progname, *filename;
int lineno;

int nerrors = 0;
int nwarnings = 0;

static char where[200];

void
ewhere (const char *format, ...) {
  va_list args;
  va_start (args, format);
  vsprintf (where, format, args);
  va_end (args);
  }

void
einfo (int type, const char *format, ...) {
  va_list args;
  int save_errno = errno;

  switch (type & 0x0f) {
  case E_NOFILE:
    fprintf (stderr, "%s: ", progname);
    break;
  case E_FILE:
    fprintf (stderr, "%s: ", filename);
    break;
  case E_FILELINE:
    ewhere ("%d", lineno);
    /* fall-through */
  case E_FILEWHERE:
    fprintf (stderr, "%s:%s: ", filename, where);
    break;
    }

  if (type & E_WARNING) {
    fprintf (stderr, "warning: ");
    nwarnings++;
    }
  else
    nerrors++;

  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);

  if (type & E_PERROR) {
    errno = save_errno;
    fprintf (stderr, ": ");
    perror ("");
    }
  else
    fprintf (stderr, "\n");
  }


int propt_tab = 30;

void
propt (const char *optname, const char *meaning) {
  if (meaning)
    if (strlen (optname) < (size_t) propt_tab)
      printf ("  %-*.*s%s\n", propt_tab, propt_tab, optname, meaning);
    else
      printf ("  %s\n  %*s%s\n", optname, propt_tab, "", meaning);
  else
    printf ("  %s\n", optname);
  }


#ifndef _GNU_SOURCE

void *
memmem (const void *buf, size_t buflen, const void *key, size_t keylen) {
  const char *s = (const char *)buf;
  const char *slim = s + (buflen - keylen + 1);
  const char key0 = *(const char *)key;
  for (; (s = memchr (s, key0, slim-s)); s++)
    if (memcmp (s, key, keylen) == 0)
      return (void *)s;

  return NULL;
  }

#endif


enum file_type
file_type (const char *fname) {
  char ext[FILENAME_MAX];
  char *s;
  const char *dot = strrchr(fname, '.');

  strcpy (ext, (dot)? dot+1 : "");
  for (s = ext; *s; s++)
    *s = tolower (*s);

  return   (strcmp (ext, "grc") == 0 || strcmp (ext, "bin") == 0)?  FT_RAW
	 : (strcmp (ext, "prc") == 0)? FT_PRC
	 : (strcmp (ext, "def") == 0)? FT_DEF
	 : FT_BFD;
  }


/* If NEWEXT is non-NULL, strips off any extension (a final `.' and all
   following characters) from FNAME and appends NEWEXT.  Returns a pointer to
   the start of the filename part (i.e., without any directories) of FNAME.  */
char *
basename_with_changed_extension (char *fname, const char *newext) {
  char *s, *dot, *dirsep;

  dot = dirsep = NULL;
  for (s = fname; *s; s++)
    if (*s == '.')  dot = s;
    else if (*s == '/' || *s == '\\')  dot = NULL, dirsep = s;

  if (newext) {
    if (dot)  strcpy (dot, newext);
    else  strcat (fname, newext);
    }

  return (dirsep)? dirsep+1 : fname;
  }


void *
slurp_file (const char *fname, const char *mode, long *sizep) {
  FILE *f;
  char *buffer;
  int text_extra = (strchr (mode, 'b') == NULL);

  if ((f = fopen (fname, mode)) == NULL)
    return NULL;

  buffer = NULL;

  if (   fseek (f, 0L, SEEK_END) != -1
      && (*sizep = ftell (f)) != -1
      && fseek (f, 0L, SEEK_SET) != -1
      && (buffer = malloc ((size_t)*sizep + text_extra)) != NULL
      && fread (buffer, 1, (size_t)*sizep, f) == (size_t)*sizep) {
    if (text_extra)
      buffer[*sizep] = '\0';
    }
  else {
    free (buffer);
    buffer = NULL;
    }

  fclose (f);
  return buffer;
  }

int
copy_file (const char *outfname, const char *infname, const char *mode) {
  char buffer[8192];
  char readmode[20], writemode[20];
  FILE *inf, *outf;
  size_t n;
  int err = 0;

  sprintf (readmode, "r%.16s", mode);
  sprintf (writemode, "w%.16s", mode);

  if ((inf = fopen (infname, readmode)) == NULL) {
    einfo (E_NOFILE | E_PERROR, "can't open `%s'", infname);
    return 0;
    }

  if ((outf = fopen (outfname, writemode)) == NULL) {
    fclose (inf);
    einfo (E_NOFILE | E_PERROR, "can't create `%s'", outfname);
    return 0;
    }

  while (!err && (n = fread (buffer, 1, sizeof buffer, inf)) > 0)
    err = (fwrite (buffer, 1, n, outf) != n);

  if (err)  einfo (E_NOFILE | E_PERROR, "error writing to `%s'", outfname);

  err = ferror (inf);
  if (err)  einfo (E_NOFILE | E_PERROR, "error reading file");

  fclose (outf);
  fclose (inf);

  return !err;
  }

void
chomp (char *s) {
  char *eos = strchr (s, '\0');
  if (eos > s && eos[-1] == '\n')  eos[-1] = '\0';
  }


char *
standard_db_type (enum database_kind kind) {
  switch (kind) {
  case DK_APPLICATION:  return "appl";
  case DK_GLIB:         return "GLib";
  case DK_SYSLIB:       return "libr";
  case DK_HACK:         return "HACK";
  default:              return NULL;
    }
  }               


#define SS_BUFSIZE  4000

struct string_store_buffer {
  struct string_store_buffer *next;
  char buffer[SS_BUFSIZE];
  };

struct string_store {
  struct string_store_buffer *first;
  char *bufp, *buflim;
  };

static void
attach_new_buffer (struct string_store *store) {
  struct string_store_buffer *b = xmalloc (sizeof (struct string_store_buffer));
  b->next = store->first;
  store->first = b;
  store->bufp = b->buffer;
  store->buflim = &b->buffer[SS_BUFSIZE];
  }

struct string_store *
new_string_store() {
  struct string_store *store = xmalloc (sizeof (struct string_store));
  store->first = NULL;
  attach_new_buffer (store);
  return store;
  }

char *
insert_string (struct string_store *store, const char *s) {
  char *t;
  int size = strlen (s)+1;

  if (store->buflim - store->bufp < size)
    attach_new_buffer (store);

  t = store->bufp;
  strcpy (store->bufp, s);
  store->bufp += size;
  return t;
  }

void
free_string_store (struct string_store *store) {
  struct string_store_buffer *b, *next;

  for (b = store->first; b; b = next) {
    next = b->next;
    free (b);
    }

  free (store);
  }
