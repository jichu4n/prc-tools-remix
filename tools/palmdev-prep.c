/* palmdev-prep.c: report on and generate paths to installed SDKs.

   Copyright 2001, 2002 John Marshall.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "libiberty.h"
#include "getopt.h"

#include "utils.h"


/* String arena used for the duration.  */
static struct string_store *store;


/* Returns true if STR starts with PREFIX, folding STR to lower case before
   comparing.  */
int
matches (const char *prefix, const char *str) {
  while (*prefix && tolower (*str) == *prefix)
    prefix++, str++;

  return *prefix == '\0';
  }


/* Returns the SDK key portion of a 'sdk-N' directory name (safely stored on
   the string store).  */
const char *
canonical_key (const char *name) {
  char *key, *dot;

  if (matches ("palmos", name))
    name += 6;
  if (matches ("sdk-", name))
    name += 4;

  key = insert_string (store, name);

  /* Canonicalise so that eg sdk-4.0 answers to "-palmos4".  */
  dot = strrchr (key, '.');
  if (dot && strcmp (dot, ".0") == 0)
    *dot = '\0';

  return key;
  }


/* FIXME autoconfiscate this.  */

#ifdef __CYGWIN__
#include <sys/cygwin.h>
#endif

/* Returns a "canonical form" of PATH.  Mostly this is just the same as
   what is given (but theoretically we'd like to make it an absolute path),
   but on Cygwin we both ensure that it's a POSIX path rather than a
   Win32-style one (which we can't quote properly in the specs file) and
   make it absolute.  */
const char *
canonical_path (const char *path) {
#ifdef __CYGWIN__
  static char posix_path[FILENAME_MAX];
  cygwin_conv_to_full_posix_path (path, posix_path);
  return posix_path;
#else
  return path;
#endif
  }


/* A ROOT is a directory with "include" and/or "lib" subdirectories, etc,
   i.e., the base of a SDK directory tree; either really a Palm OS SDK
   (e.g., /opt/palmdev/sdk-3.5) or the generic SDK-neutral part of a
   PalmDev tree (e.g. /opt/palmdev).

   A series of analyze_palmdev_tree() calls creates two linked lists of
   roots:  GENERIC_ROOT_LIST is an ordered list of the generic ones, while
   SDK_ROOT_LIST contains the ones that are real SDKs and is really a
   table accessed with find().  The KEY and BASE fields are only used by
   the latter.  */

struct root {
  struct root *next;
  const char *prefix;	/* Full path of the root directory.  */
  const char *include;	/* Name of the headers subdirectory, if any.  */
  const char *lib;	/* Name of the libraries subdirectory, if any.  */
  const char *key;	/* Canonical SDK key, in the case of a SDK root.  */
  const char *base;	/* Key of SDK upon which this SDK is based, if any.  */
  };

struct root *
make_root (int base_allowed, const char *path_format, ...) {
  char path[FILENAME_MAX];
  FILE *basef;
  va_list args;
  struct root *root = xmalloc (sizeof (struct root));

  va_start (args, path_format);
  vsprintf (path, path_format, args);
  va_end (args);

  root->next = NULL;
  root->prefix = insert_string (store, path);
  root->include = is_dir ("%s/include", path)? "include"
		: is_dir ("%s/Incs", path)? "Incs"
		: NULL;
  root->lib = is_dir ("%s/lib", path)? "lib"
	    : is_dir ("%s/GCC Libraries", path)? "GCC Libraries"
	    : NULL;

  root->base = NULL;
  strcat (path, "/base");
  if (base_allowed && (basef = fopen (path, "r")) != NULL) {
    char text[80];
    if (fgets (text, sizeof text, basef)) {
      char *nl = strchr (text, '\n');
      if (nl)  *nl = '\0';
      root->base = canonical_key (text);
      }
    fclose (basef);
    }

  return root;
  }

struct root *
find (struct root *list, const char *key) {
  if (key)
    for (; list; list = list->next)
      if (strcmp (key, list->key) == 0)
	return list;

  return NULL;
  }

void
free_root_list (struct root *list) {
  while (list) {
    struct root *next = list->next;

    /* All the strings in *LIST are allocated in the string_store, so need
       not be explicitly freed here.  */
    free (list);
    list = next;
    }
  }


struct root *generic_root_list, *sdk_root_list;

int
sdk_valid (const struct root *sdk) {
  /* An SDK is valid if you can see some headers through it, either directly
     or via a base SDK.  If it has a base, we merely assume that (somewhere
     down the line) the base is valid; if not, we'll get a warning for the
     most basic base.  */
  return sdk->include || sdk->base;
  }

void
print_report (const char *name, const struct root *root,
	      const struct root *overriding_root, int headers_required) {
  printf ("  %-13s\t", name);

  if (overriding_root)
    printf ("UNUSED -- hidden by %s", overriding_root->prefix);
  else if (headers_required && ! sdk_valid (root))
    printf ("INVALID -- no headers");
  else {
    if (root->include)
      printf ("headers in '%s'", root->include);
    else
      printf ("no headers");

    if (root->lib)
      printf (", libraries in '%s'", root->lib);
    else
      printf (", no libraries");

    if (root->base)
      printf (", based on sdk-%s", root->base);
    }

  printf ("\n");
  }

void
analyze_palmdev_tree (const char *prefix_as_given, int report) {
  static struct root **last_generic_root = &generic_root_list;

  const char *prefix = canonical_path (prefix_as_given);
  DIR *dir = opendir (prefix);

  if (dir) {
    struct dirent *e;
    struct root *root;
    int n = 0;

    if (report)
      printf ("Checking SDKs in %s\n", prefix_as_given);

    while ((e = readdir (dir)) != NULL)
      if (matches ("sdk-", e->d_name) &&
	  is_dir_dirent (e, "%s/%s", prefix, e->d_name)) {
	const char *key = canonical_key (e->d_name);
	struct root *overriding_sdk = find (sdk_root_list, key);

	if (overriding_sdk)
	  root = NULL;
	else
	  root = make_root (1, "%s/%s", prefix, e->d_name);

	n++;
	if (report)
	  print_report (e->d_name, root, overriding_sdk, 1);

	if (root) {
	  if (sdk_valid (root)) {
	    root->key = key;
	    root->next = sdk_root_list;
	    sdk_root_list = root;
	    }
	  else
	    free (root);
	  }
	}

    closedir (dir);

    if (report && n == 0)
      printf ("  (none)\n");

    root = make_root (0, "%s", prefix);
    if (root->include || root->lib) {
      if (report) {
	printf ("  and material in %s used regardless of SDK choice\n",
		prefix_as_given);
	print_report ("  (common)", root, NULL, 0);
	}

      *last_generic_root = make_root (0, "%s", prefix);
      last_generic_root = &((*last_generic_root)->next);
      }
    else
      free (root);

    if (report)
      printf ("\n");
    }
  }


struct spec_kind {
  const char *spec;
  void (*write_tree) (FILE *, const struct root *, const struct spec_kind *);
  const char * const *targetdirs;
  };

static void
write_option (FILE *f, const char *option, const char *path) {
  const char *s;
  fprintf (f, " %s", option);
  for (s = path; *s; s++) {
    if (isspace (*s))
      putc ('\\', f);
    putc (*s, f);
    }
  }

static void
write_include_tree (FILE *f, const struct root *root,
		    const struct spec_kind *kind UNUSED_PARAM) {
  if (root->include) {
    TREE *tree = opentree (DIRS_PREORDER, "%s/%s", root->prefix, root->include);
    const char *dir;
    while ((dir = readtree (tree)) != NULL)
      write_option (f, "-isystem ", dir);
    closetree (tree);
    }
  }

static void
write_lib_tree (FILE *f, const struct root *root,
		const struct spec_kind *kind) {
  if (root->lib) {
    const char * const *targetdir;
    for (targetdir = kind->targetdirs; *targetdir; targetdir++) {
      TREE *tree = opentree (DIRS_PREORDER,
			     "%s/%s/%s", root->prefix, root->lib, *targetdir);
      const char *dir;

      while ((dir = readtree (tree)) != NULL) {
	/* FIXME check for multi-libs.  */
	write_option (f, "-L", dir);
	}

      closetree (tree);
      }
    }
  }

static void
write_sdk_spec (FILE *f, const struct root *sdk, const struct spec_kind *kind) {
  fprintf (f, "*%s_sdk_%s:\n", kind->spec, sdk->key);
  kind->write_tree (f, sdk, kind);
  if (sdk->base)
    fprintf (f, " %%(%s_sdk_%s)", kind->spec, sdk->base);
  fprintf (f, "\n\n");
  }

static void
write_main_spec (FILE *f, const struct root *default_sdk,
		 const struct spec_kind *kind) {
  struct root *root, *sdk;

  fprintf (f, "*%s:\n+ %%{!palmos-none:", kind->spec);

  for (root = generic_root_list; root; root = root->next)
    kind->write_tree (f, root, kind);

  for (sdk = sdk_root_list; sdk; sdk = sdk->next) {
    fprintf (f, " %%{palmos%s:%%(%s_sdk_%s)}", sdk->key, kind->spec, sdk->key);
    if (strspn (sdk->key, "0123456789") == strlen (sdk->key))
      fprintf (f, " %%{palmos%s.0:%%(%s_sdk_%s)}", sdk->key, kind->spec,
	       sdk->key);
    }

  if (default_sdk)
    fprintf (f, " %%{!palmos*: %%(%s_sdk_%s)}", kind->spec, default_sdk->key);

  fprintf (f, "}\n\n");
  }

void
write_specs (FILE *f, const char *target, const struct root *default_sdk) {
  static const struct spec_kind include = { "cpp", write_include_tree, NULL };
  static const char * const m68k_libdirs[] = { "m68k-palmos-coff", NULL };

  const char * const generic_libdirs[] = { target, NULL };
  const struct spec_kind lib = {
    "link", write_lib_tree,
    matches ("m68k-", target)? m68k_libdirs : generic_libdirs
    };

  struct root *sdk;

  for (sdk = sdk_root_list; sdk; sdk = sdk->next) {
    write_sdk_spec (f, sdk, &include);
    write_sdk_spec (f, sdk, &lib);
    }

  write_main_spec (f, default_sdk, &include);
  write_main_spec (f, default_sdk, &lib);
  }


/* Palm OS trap vectors (as expressed in CoreTraps.h) start at 0xa000 and
   currently go up to about 0xa480.  So these limits are ample.  */

#define TRAPNO_MIN  0xa000
#define TRAPNO_MAX  0xafff

void
write_traps (FILE *f, char *s) {
  char *slim;
  const char *key, *trap[TRAPNO_MAX - TRAPNO_MIN + 1];
  unsigned int i, n, value, value_min, value_max;

  for (i = TRAPNO_MIN; i <= TRAPNO_MAX; i++)
    trap[i - TRAPNO_MIN] = NULL;

  n = 0;
  value_min = TRAPNO_MAX;
  value_max = TRAPNO_MIN;

  /* Find occurences of "#<ws>define<ws>sysTrap<word><ws><number>".
     Thus we're assuming that any comments occur *after* the <number>;
     this is true of the existing SDK headers.  */

  while ((s = strchr (s, '#')) != NULL) {
    s++;
    while (isspace (*s))  s++;
    if (strncmp (s, "define", sizeof "define" - 1) != 0)
      continue;

    s += sizeof "define" - 1;
    while (isspace (*s))  s++;
    if (strncmp (s, "sysTrap", sizeof "sysTrap" - 1) != 0)
      continue;

    s += sizeof "sysTrap" - 1;

    key = s;
    while (! isspace (*s))  s++;
    *s++ = '\0';

    value = strtoul (s, &slim, 0);

    if (strcmp (key, "Base") != 0 && strcmp (key, "LastTrapNumber") != 0
	&& slim > s && value >= TRAPNO_MIN && value <= TRAPNO_MAX) {
      trap[value - TRAPNO_MIN] = key;
      n++;
      if (value > value_max)  value_max = value;
      if (value < value_min)  value_min = value;
      }

    s = slim;
    }

  fprintf (f,
    "Total number of traps present, and minimum and maximum trap vectors:\n"
    "* %u 0x%x 0x%x\n\n", n, value_min, value_max);

  for (i = value_min; i <= value_max; i++)
    if (trap[i - TRAPNO_MIN])
      fprintf (f, "0x%x %s\n", i, trap[i - TRAPNO_MIN]);
  }


FILE *
fopen_for_writing (const char *fname, const char **message) {
  FILE *f = fopen (fname, "w");
  if (f == NULL) {
#ifdef EACCES
    if (errno == EACCES)
      *message =
	"Permission to write configuration files denied -- try again as root";
#endif
    error ("can't write to '%s': @P", fname);
    }

  return f;
  }

void
remove_file (int verbose, const char *fname) {
  struct stat st;

  if (stat (fname, &st) != 0)
    return;  /* Already non-existent.  */

  if (remove (fname) == 0) {
    if (verbose)
      printf ("Removed '%s'\n", fname);
    }
  else
    warning ("can't remove '%s': @P", fname);
  }


const char *
next_target (DIR **dir) {
  struct dirent *e;

  if (*dir == NULL)
    *dir = opendir (STANDARD_EXEC_PREFIX);

  while (*dir && (e = readdir (*dir)) != NULL)
    if (strstr (e->d_name, "-palmos")
	&& is_dir_dirent (e, "%s/%s", STANDARD_EXEC_PREFIX, e->d_name))
      return e->d_name;

  if (*dir)
    closedir (*dir);

  return NULL;
  }

char *
specfilename (const char *target) {
  static char fname[FILENAME_MAX];
  sprintf (fname, "%s/%s/specs", STANDARD_EXEC_PREFIX, target);
  return fname;
  }


void
usage () {
  printf ("Usage: %s [options] [directory...]\n", progname);
  printf ("Directories listed will be scanned in addition to %s\n",
	  PALMDEV_PREFIX);
  printf ("Options:\n");
  propt ("-d SDK, --default SDK", "Set default SDK");
  propt ("-r, --remove", "Remove all files installed by palmdev-prep");
  propt ("--dump-specs TARGET", "Write specs for TARGET to standard output");
  propt ("-q, --quiet, --silent", "Suppress display of installation analysis");
  propt ("-v, --verbose", "Display extra information about actions taken");
  }

static int show_help, show_version;

static const char shortopts[] = "d:rqv";

static struct option longopts[] = {
  { "default", required_argument, NULL, 'd' },
  { "remove", no_argument, NULL, 'r' },
  { "dump-specs", required_argument, NULL, 'D' },

  { "quiet", no_argument, NULL, 'q' },
  { "silent", no_argument, NULL, 'q' },
  { "verbose", no_argument, NULL, 'v' },

  { "help", no_argument, &show_help, 1 },
  { "version", no_argument, &show_version, 1 },
  { NULL, no_argument, NULL, 0 }
  };

int
main (int argc, char **argv) {
  static const char trapnumbers_fname[] = DATA_PREFIX"/trapnumbers";

  const char *default_sdk_name = NULL;
  const char *dump_target = NULL;
  int removing = 0, report = 1, verbose = 0;
  int c;

  set_progname (argv[0]);

  while ((c = getopt_long (argc, argv, shortopts, longopts, NULL)) >= 0)
    switch (c) {
    case 'd':
      default_sdk_name = optarg;
      break;

    case 'D':
      dump_target = optarg;
      break;

    case 'r':
      removing = 1;
      break;

    case 'q':
      report = 0;
      break;

    case 'v':
      verbose = 1;
      break;

    case 0:
      /* 0 indicates an automatically handled long option: do nothing.  */
      break;

    default:
      nerrors++;
      show_help = 1;
      break;
      }

  if (show_version || show_help) {
    if (show_version)
      print_version ("palmdev-prep", "Jg");

    if (show_help)
      usage ();
    }
  else if (removing) {
    const char *target;
    DIR *dir = NULL;

    while ((target = next_target (&dir)) != NULL)
      remove_file (verbose, specfilename (target));

    remove_file (verbose, trapnumbers_fname);
    }
  else {
    struct root *default_sdk = NULL;
    struct root *sdk;

    store = new_string_store ();

    analyze_palmdev_tree (PALMDEV_PREFIX, report);
    for (; optind < argc; optind++)
      if (is_dir ("%s", argv[optind]))
	analyze_palmdev_tree (argv[optind], report);
      else
	warning ("can't open '%s': @P", argv[optind]);

    for (sdk = sdk_root_list; sdk; sdk = sdk->next)
      if (sdk->base && ! find (sdk_root_list, sdk->base)) {
	warning ("[%s/base] base SDK '%s' not found", sdk->prefix, sdk->base);
	sdk->base = NULL;
	}

    if (default_sdk_name) {
      default_sdk = find (sdk_root_list, canonical_key (default_sdk_name));
      if (default_sdk == NULL)
	warning ("SDK '%s' not found -- using highest found instead",
		 default_sdk_name);
      }

    if (default_sdk == NULL && sdk_root_list) {
      /* Find the SDK with the alphabetically highest key.  */
      default_sdk = sdk_root_list;
      for (sdk = sdk_root_list; sdk; sdk = sdk->next)
	if (strcmp (sdk->key, default_sdk->key) > 0)
	  default_sdk = sdk;

      if (report)
	printf ("When GCC is given no -palmos options, "
		"SDK '%s' will be used by default\n\n", default_sdk->key);
      }

    if (dump_target)
      write_specs (stdout, dump_target, default_sdk);
    else if (generic_root_list || sdk_root_list) {
      const char *target, *header_fname;
      int ntargets;
      DIR *dir = NULL;
      const char *message = "...done";

      if (report)
	printf ("Writing SDK details to configuration files...\n");

      for (ntargets = 0; (target = next_target (&dir)) != NULL; ntargets++) {
	const char *fname = specfilename (target);
	FILE *f = fopen_for_writing (fname, &message);

	if (f) {
	  write_specs (f, target, default_sdk);
	  fclose (f);

	  if (verbose)
	    printf ("Wrote %s specs to '%s'\n", target, fname);
	  }
	}

      remove_file (0, trapnumbers_fname);

      for (header_fname = NULL, sdk = default_sdk;
	   header_fname == NULL && sdk;
	   sdk = find (sdk_root_list, sdk->base))
	if (sdk->include) {
	  TREE *tree = opentree (FILES, "%s/%s", sdk->prefix, sdk->include);
	  while ((header_fname = readtree (tree)) != NULL) {
	    const char *base = lbasename (header_fname);
	    if (matches ("coretraps.h", base) || matches ("systraps.h", base))
	      break;
	    }
	  closetree (tree);
	  }

      if (header_fname) {
	long header_size;
	char *header_text = slurp_file (header_fname, "r", &header_size);
	if (header_text) {
	  FILE *f = fopen_for_writing (trapnumbers_fname, &message);
	  if (f) {
	    fprintf (f, "Palm OS trap vectors from '%s'\n\n", header_fname);
	    write_traps (f, header_text);
	    fclose (f);

	    if (verbose)
	      printf ("Parsed trap numbers in '%s'\n"
		      "  and wrote them to '%s'\n",
		      header_fname, trapnumbers_fname);
	    }

	  free (header_text);
	  }
	else
	  warning ("can't open '%s': @P", header_fname);
	}

      if (report)
	printf ("%s\n", message);

      if (ntargets == 0)
	error ("installation problem: no prc-tools targets found");
      }
    else {
      error ("no Palm OS development material detected");
      if (report)
	printf ("\n"
"Either make SDKs and other material available at %s\n"
"and rerun palmdev-prep, or specify a directory in which they can be found\n"
"when you rerun palmdev-prep.\n"
"See <URL:http://prc-tools.sourceforge.net/install/> for details.\n",
		PALMDEV_PREFIX);
      }

    free_root_list (generic_root_list);
    free_root_list (sdk_root_list);

    free_string_store (store);
    }

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
