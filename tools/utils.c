/* utils.c: various utilities.

   Copyright 1998, 1999, 2001, 2002 John Marshall.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "libiberty.h"

#include "utils.h"

const char *progname;

void
set_progname (const char *progname0) {
  /* FIXME: possibly might want to do a basename() here.  */
  progname = progname0;
  xmalloc_set_program_name (progname);
  }


int nerrors = 0;
int nwarnings = 0;

static void
print_diagnostic (const char *format, const char *warnstr, va_list *args,
		  int saved_errno) {
  static char *fmt = NULL;
  static size_t fmt_len = 0;

  int use_perror;
  size_t needed_len = strlen (format) + strlen (warnstr) + 20;

  if (needed_len > fmt_len) {
    free (fmt);
    fmt_len = needed_len;
    fmt = xmalloc (fmt_len);
    }

  if (format[0] == '[') {
    int ket_pos;

    format++;
    ket_pos = strcspn (format, "]");

    strcpy (fmt, format);
    fmt[ket_pos] = '\0';
    strcat (fmt, ": ");
    format += ket_pos + 1;
    while (*format == ' ')  format++;
    }
  else {
    fprintf (stderr, "%s: ", progname);
    strcpy (fmt, "");
    }

  strcat (fmt, warnstr);
  strcat (fmt, format);

  if (strlen (fmt) >= 2 && strcmp (fmt + strlen (fmt) - 2, "@P") == 0) {
    fmt[strlen (fmt) - 2] = '\0';
    use_perror = 1;
    }
  else
    use_perror = 0;

  vfprintf (stderr, fmt, *args);

  if (use_perror) {
    errno = saved_errno;
    perror ("");
    }
  else
    putc ('\n', stderr);
  }

void
error (const char *format, ...) {
  int saved_errno = errno;
  va_list args;
  va_start (args, format);
  print_diagnostic (format, "", &args, saved_errno);
  va_end (args);
  nerrors++;
  }

void
warning (const char *format, ...) {
  int saved_errno = errno;
  va_list args;
  va_start (args, format);
  print_diagnostic (format, "warning: ", &args, saved_errno);
  va_end (args);
  nwarnings++;
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


void
print_version (const char *canonical_progname, const char *flags) {
  static const struct holder { char key; const char *name; } holders[] = {
    { 'g', "Free Software Foundation, Inc" },
    { 'j', "John Marshall" },
    { 'p', "Palm, Inc. or its subsidiaries" },
    { '\0', NULL }
    };

  static const int year = 2002;

  const char *s;

  printf ("%s (", canonical_progname);
  printf ("prc-tools) %s\n", PRC_TOOLS_VERSION);

  for (s = flags; *s; s++) {
    char holder_key = tolower (*s);
    const struct holder *h;
    for (h = holders; h->key; h++)
      if (h->key == holder_key) {
	printf ("%s %d %s.\n",
		(isupper (*s))? "Copyright" : "Portions copyright", year,
		h->name);
	break;
	}
    }

  printf (
"This program is free software; you may redistribute it under the terms of\n"
"the GNU General Public License.  This program has absolutely no warranty.\n");
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


/* If NEWEXT is non-NULL, strips off any extension (a final '.' and all
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
    error ("can't open '%s': @P", infname);
    return 0;
    }

  if ((outf = fopen (outfname, writemode)) == NULL) {
    fclose (inf);
    error ("can't create '%s': @P", outfname);
    return 0;
    }

  while (!err && (n = fread (buffer, 1, sizeof buffer, inf)) > 0)
    err = (fwrite (buffer, 1, n, outf) != n);

  if (err)  error ("error writing to '%s': @P", outfname);

  err = ferror (inf);
  if (err)  error ("error reading file: @P");

  fclose (outf);
  fclose (inf);

  return !err;
  }


void
generate_file_from_template (const char *fname, const char *const *tmpl,
			     int (*filter)(FILE *f, const char *key)) {
  FILE *f = fopen (fname, "w");
  const char *const *str;

  if (f == NULL) {
    error ("can't create '%s': @P", fname);
    return;
    }

  for (str = tmpl; *str; str++)
    if (strcmp (*str, "@progname@") == 0)
      fprintf (f, "%s v%s", progname, PRC_TOOLS_VERSION);
    else if (strcmp (*str, "@fname@") == 0)
      fprintf (f, "%s", fname);
    else if (filter (f, *str))
      ;
    else
      fprintf (f, "%s", *str);

  if (fclose (f) != 0) {
    error ("can't close '%s': @P", fname);
    remove (fname);
    }
  }


void
chomp (char *s) {
  char *eos = strchr (s, '\0');
  if (eos > s && eos[-1] == '\n')  eos[-1] = '\0';
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
