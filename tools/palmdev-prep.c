/* palmdev-prep.c: report on and generate paths to installed SDKs.

   Copyright 2001, 2002 John Marshall.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include "config.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "libiberty.h"
#include "getopt.h"

#include "utils.h"


/* Options used in various actions (most are 0 / NULL at startup):  */

const char *default_sdk_key;	/* Which SDK to be used sans -palmos options */
const char *dump_target;	/* Determines which target's specs are dumped */
int report = 1;			/* Whether to show analysis of directories */
int verbose;			/* Whether to echo commands before execution */
int alter_toplevel_palmdev;	/* Whether to create or rm <palmdev> itself */

int show_help, show_version;	/* Show help and version messages instead */

static int nextra_trees;	/* Count of additional trees on command line */
static char **extra_tree_paths;	/* ...and their paths */


/* If we had closures, we would pass parameters down through our various
   iterator functions.  But in a C program of this (small) size, it's easier
   just to have them as globals.  BASIC programmers reunite!  */

struct sdk *all_sdks;		/* Mapping of SDK keys to directory paths.  */

struct string_store *store;	/* General-purpose autofreed strdup arena.  */

void
for_each_palmdev_tree (void (*process) (const char *path)) {
  static int first_invocation = 1;

  int i;

  if (is_dir ("%s", PALMDEV_PREFIX))
    process (PALMDEV_PREFIX);

  for (i = 0; i < nextra_trees; i++)
    if (is_dir ("%s", extra_tree_paths[i]))
      process (extra_tree_paths[i]);
    else
      if (first_invocation)
	error ("PalmDev tree '%s' does not exist", extra_tree_paths[i]);

  first_invocation = 0;
  }


void
canonicalise_sdk_key (char *key) {
  char *dot = strrchr (key, '.');

  /* Canonicalise so that eg sdk-4.0 answers to "-palmos4".  */
  if (dot && strcmp (dot, ".0") == 0)
    *dot = '\0';
  }

const char *
skip_sdk_prefix (const char *name) {
  char prefix[5];
  int i;

  /* On a non-case-sensitive file system, a subdirectory might be returned as
     "SDK-1" with no real way for the user to uncapitalise it.  */
  for (i = 0; i < 4 && name[i] != '\0'; i++)
    prefix[i] = tolower (name[i]);
  prefix[i] = '\0';

  return (strcmp (prefix, "sdk-") == 0)? &name[4] : NULL;
  }




/* It would be nice to implement for_each_sdk() as
     for_each_palmdev_tree (for_each_subdir (if toplevel and matches sdk-X ()))
   but the iteration would need to remember state because a "sdk-foo" directory
   in a later tree is hidden by "sdk-foo" in an earlier tree.  Since we need
   to remember them anyway, we store a whole list of SDKs.  This has the
   cosmetic bonus that we get to sort them before use.  */

struct sdk {
  struct sdk *next;
  const char *key, *path;
  };

/* These variables are only for communication between insert_each_sdk_subdir
   and insert_each_sdk_subdir_aux.  */

static struct sdk *insert_each_sdk_subdir_list;
static int insert_each_sdk_subdir_dups;

static int
insert_each_sdk_subdir_aux (const char *path, const char *base) {
  const char *rawkey = skip_sdk_prefix (base);
  if (rawkey) {
    char key[FILENAME_MAX];
    struct sdk **prevs_next = &insert_each_sdk_subdir_list;
    struct sdk *sdk = insert_each_sdk_subdir_list;

    strcpy (key, rawkey);
    canonicalise_sdk_key (key);

    while (sdk && strcmp (sdk->key, key) < 0) {
      prevs_next = &sdk->next;
      sdk = sdk->next;
      }

    if (insert_each_sdk_subdir_dups || ! (sdk && strcmp (sdk->key, key) == 0)) {
      struct sdk *newsdk = xmalloc (sizeof (struct sdk));
      newsdk->key = insert_string (store, key);
      newsdk->path = insert_string (store, path);
      newsdk->next = sdk;
      *prevs_next = newsdk;
      }
    }

  return 0;
  }

struct sdk *
insert_each_sdk_subdir (struct sdk *list, int insert_dups, const char *path) {
  insert_each_sdk_subdir_list = list;
  insert_each_sdk_subdir_dups = insert_dups;
  for_each_subdir (insert_each_sdk_subdir_aux, 0, "%s", path);
  return insert_each_sdk_subdir_list;
  }

void
free_sdk_list (struct sdk *list) {
  while (list) {
    struct sdk *next = list->next;
    free (list);
    list = next;
    }
  }

struct sdk *
find_sdk (struct sdk *list, const char *key) {
  while (list) {
    if (strcmp (list->key, key) == 0)
      return list;
    list = list->next;
    }

  return NULL;
  }

void
for_each_sdk (const struct sdk *list,
	      void (*process) (const char *key, const char *path)) {
  while (list) {
    process (list->key, list->path);
    list = list->next;
    }
  }


void
for_each_installed_target (void (*process) (const char *target)) {
  /* FIXME: When we get multiple targets, we should do some configury to
     do this, or just iterate over the subdirectories of $libdir/gcc-lib
     that match /-palmos/.  */

  process (TARGET_ALIAS);
  }


const char *
targetlibdir (const char *t) {
  if (strstr (t, "m68k-"))
    return "m68k-palmos-coff";  /* Account for hysterical raisins.  */
  else
    return t;
  }


static FILE *specf;
static const char *spectarget;


void
print_armoured (const char *s) {
  for (; *s; s++) {
    if (isspace (*s))
      putc ('\\', specf);
    putc (*s, specf);
    }
  }

int
print_isystem (const char *path, const char *base) {
  fprintf (specf, " -isystem ");
  print_armoured (path);
  return 1;
  }

int
print_l (const char *path, const char *base) {
  fprintf (specf, " -L");
  print_armoured (path);
  return 1;
  }

void
print_includedirs (const char *path) {
  for_each_subdir (print_isystem, 1, "%s/%s", path,
		   is_dir ("%s/include", path)? "include" : "Incs");
  }

// FIXME: targets! multilibs!
void
print_targetlibdirs (const char *path) {
  for_each_subdir (print_l, 1, "%s/%s/%s", path,
		   is_dir ("%s/lib", path)? "lib" : "GCC Libraries",
		   targetlibdir (spectarget));
  }





static const char *spec_kind;

void
print_palmos_flags (const char *key, const char *path) {
  fprintf (specf, " %%{palmos%s:%%(%s_sdk_%s)}", key, spec_kind, key);

  if (! strchr (key, '.'))
    fprintf (specf, " %%{palmos%s.0:%%(%s_sdk_%s)}", key, spec_kind, key);
  }

void
print_main_spec (const char *kind, void (*process) (const char *path)) {
  fprintf (specf, "*%s:\n+ %%{!palmos-none:", kind);
  for_each_palmdev_tree (process);
  spec_kind = kind;
  for_each_sdk (all_sdks, print_palmos_flags);
  fprintf (specf, " %%{!palmos*:%%(%s_sdk_%s)}}\n\n", kind, default_sdk_key);
  }

void
print_sdk_specs (const char *key, const char *path) {
  fprintf (specf, "*cpp_sdk_%s:\n", key);
  print_includedirs (path);
  fprintf (specf, "\n\n");

  fprintf (specf, "*link_sdk_%s:\n", key);
  print_targetlibdirs (path);
  fprintf (specf, "\n\n");
  }

void
gen_specs (FILE *f, const char *target0) {
  specf = f;
  spectarget = target0;

  for_each_sdk (all_sdks, print_sdk_specs);

  print_main_spec ("cpp", print_includedirs);
  print_main_spec ("link", print_targetlibdirs);
  }


/* These variables are only for communication between count_targets and
   count_targets_aux.  */

static const char *count_targets_path1;
static const char *count_targets_path2;
static int count_targets_count;

static void
count_targets_aux (const char *target) {
  if (is_dir ("%s/%s/%s", count_targets_path1, count_targets_path2,
	      targetlibdir (target)))
    count_targets_count++;
  }

int
count_targets (const char *path1, const char *path2) {
  count_targets_path1 = path1;
  count_targets_path2 = path2;
  count_targets_count = 0;
  for_each_installed_target (count_targets_aux);
  return count_targets_count;
  }


static int report_hidden, report_Incs, report_no_includes,
	   report_GCC_Libraries, report_no_library_targets;

void
report_headers_libs (const char *key, const char *path) {
  const char *sep = strrchr (path, '/');
  const char *base = sep? sep + 1 : path;
  struct sdk *sdk = key? find_sdk (all_sdks, key) : NULL;

  printf ("  %-13s\t", key? base : "general");

  if (sdk && strcmp (sdk->path, path) != 0) {
    printf ("UNUSED -- hidden by %s\n", sdk->path);
    report_hidden = 1;
    }
  else {
    const char *includes, *libs;

    if (is_dir ("%s/include", path))
      includes = "include";
    else if (is_dir ("%s/Incs", path)) {
      includes = "Incs";
      report_Incs = 1;
      }
    else
      includes = NULL;

    if (is_dir ("%s/lib", path))
      libs = "lib";
    else if (is_dir ("%s/GCC Libraries", path)) {
      libs = "GCC Libraries";
      report_GCC_Libraries = 1;
      }
    else
      libs = NULL;

    if (includes)
      printf ("headers in '%s', ", includes);
    else {
      if (key) {
	printf ("INVALID -- no headers; ");
	report_no_includes = 1;
	}
      else
	printf ("no headers, ");
      }

    if (libs) {
      int ntargets = count_targets (path, libs);
      switch (ntargets) {
      case 0:
	printf ("NO LIBRARIES in '%s'\n", libs);
	report_no_library_targets = 1;
	break;
      case 1:
	printf ("libraries (one target) in '%s'\n", libs);
	break;
      default:
	printf ("libraries (%d targets) in '%s'\n", ntargets, libs);
	break;
	}
      }
    else {
      /* No report for this, becaues old SDKs didn't have any libraries.  */
      printf ("no libraries\n");
      }
    }
  }

static int report_hidden, report_Incs, report_no_includes,
	   report_GCC_Libraries, report_no_library_targets;

void
report_tree (const char *path) {
  struct sdk *sdks = insert_each_sdk_subdir (NULL, 1, path);
  printf ("%s\n", path);
  report_headers_libs (NULL, path);
  for_each_sdk (sdks, report_headers_libs);
  free_sdk_list (sdks);
  }

/* May have reason to use getmntent and cygwin_conv_to_posix_path */

/* Instead of wrapping with sdkfind:

   palmdev-prep writes to $prefix/lib/gcc-lib/m68k-palmos/specs
   (which is a new file, different from GCC's  ...-palmos/2.95.3-kgpd/specs) */



void
usage () {
  printf ("Usage: %s [options] [directory...]\n", progname);
  printf ("Options:\n");
  propt ("-i, --install [or by default]",
	 "Install basic PalmDev framework and specs files");
  propt ("-d SDK, --default SDK", "Set default SDK");
  propt ("--dump-specs TARGET", "Write specs for target to standard output");
  propt ("-q, --quiet, --silent", "Suppress display of installation analysis");
  propt ("-r, --remove", "Remove installed files and directories");
  propt ("-R, --remove-palmdev", "  including " PALMDEV_PREFIX);
  propt ("-v, --verbose", "Display invoked subcommands");
  printf ("PalmDev tree:\n  %s\n", PALMDEV_PREFIX);
  }

static const char shortopts[] = "d:iqrRv";

static struct option longopts[] = {
  { "install", no_argument, NULL, 'i' },
  { "default", required_argument, NULL, 'd' },
  { "remove", no_argument, NULL, 'r' },
  { "remove-palmdev", no_argument, NULL, 'R' },
  { "dump-specs", required_argument, NULL, 'D' },

  { "quiet", no_argument, NULL, 'q' },
  { "silent", no_argument, NULL, 'q' },
  { "verbose", no_argument, NULL, 'v' },

  { "help", no_argument, &show_help, 1 },
  { "version", no_argument, &show_version, 1 },
  { NULL, no_argument, NULL, 0 }
  };


#if 0
int
for_each_frameworkdir (const char *prefix, int (*process) (const char *path)) {
  char path[FILENAME_MAX];

  process (prefix);

  sprintf (path, "%s/include", prefix);
  process (path);

  sprintf (path, "%s/lib", prefix);
  process (path);

  for_each_installed_target (for_each_frameworkdir_aux, process);
  }

#endif



/* Install searches <palmdev> and any supplied directory trees for SDKs,
   printing analyses of what it finds (unless --quiet), and writes specs
   files accordingly (if it has write permission).

   If --install is explicitly given, or if <palmdev> itself exists, or if
   we're on Cygwin and there's a <palmdev> mount entry (even without a
   directory behind it), we also create the basic directory framework under
   <palmdev>.  */

char *
specfilename (const char *target) {
  static char fname[FILENAME_MAX];
  sprintf (fname, "%s/%s/specs", STANDARD_EXEC_PREFIX, target);
  return fname;
  }


static int install_specfile_access_denied;

static void
install_specfile (const char *target) {
  const char *fname = specfilename (target);
  FILE *f = fopen (fname, "w");

  if (f) {
    gen_specs (f, target);
    fclose (f);

    if (verbose)
      printf ("Wrote %s specs to '%s'\n", target, fname);
    }
  else {
#ifdef EACCES
    if (errno == EACCES)
      install_specfile_access_denied++;
#endif
    error ("can't write to '%s': @P", fname);
    }
  }


void
do_install () {
  if (alter_toplevel_palmdev || is_dir ("%s", PALMDEV_PREFIX)) {
    /* create framework */
    }

  if (report) {
    for_each_palmdev_tree (report_tree);
    printf ("\nWriting SDK details to target spec files...\n");
    }

  install_specfile_access_denied = 0;
  for_each_installed_target (install_specfile);
  if (report) {
    if (install_specfile_access_denied)
      printf ("Permission to write spec files denied -- try again as root\n");
    else
      printf ("...done\n");
    }

#if 0
  char *palmdev_main = strtok (palmdev_path, ":");
  struct string_list *all = list_dirtree (palmdev_main);
  struct string_list *p;

  printf ("PalmDev path: %s\n", palmdev_path);

  if (! check_framework (palmdev_main, 0)) {
    printf ("Creating basic PalmDev directories in %s:\n", palmdev_main);
    check_framework (palmdev_main, 1);
    }

  // process optind..argc
  printf ("Doing stuff\n");

  sort_string_list (&all, strcmp);

  printf ("Dirs:\n");
  for (p = all; p; p = p->next)
    printf ("  %s\n", p->text);

  free_string_list (all);
#endif
  }


/* Dump just generates one set of specs, and always has permission to
   write (since it's to stdout). */

void
do_dumpspec () {
  gen_specs (stdout, dump_target);
  }


int
exists (const char *fname) {
  struct stat st;
  return stat (fname, &st) == 0;
  }

/* Remove deletes any specs files we've created, and deletes the basic
   framework under <palmdev>, as long as those basic directories are empty.  */

static void
remove_specfile (const char *target) {
  const char *fname = specfilename (target);

  if (! exists (fname))
    return;

  if (remove (fname) == 0) {
    if (verbose)
      printf ("Removed '%s'\n", fname);
    }
  else {
    }
  }

static void
remove_targetlibdir (const char *target) {
  }

void
do_remove () {
  for_each_installed_target (remove_specfile);
  for_each_installed_target (remove_targetlibdir);
  }




int
create_dir (const char* dir) {
  if (mkdir (dir, 0755) == 0)
    if (chmod (dir, 0755) == 0)
      return 1;
    else
      error ("can't chmod '%s': @P", dir);
  else
    error ("can't create '%s': @P", dir);

  return 0;
  }

#if 0
int
check_framework (const char *prefix, int create) {
  static const char* subdirs[] = {
    "include",
    "lib/m68k-palmos-coff",
    NULL
    };

  const char **subdir;

  for (subdir = subdirs; *subdir; subdir++)
    if (! is_dir ("%s/%s", prefix, *subdir)) {
      if (create) {
	char dir[FILENAME_MAX], partial[FILENAME_MAX];
	char *d;
	sprintf (dir, "%s/%s", prefix, *subdir);
	strcpy (partial, "");
	for (d = strtok (dir, "/"); d; d = strtok (NULL, "/")) {
	  strcat (partial, "/");
	  strcat (partial, d);
	  if (! is_dir (partial))
	    if (! create_dir (partial))
		return 0;
	  }
	}
      else
	return 0;
      }

  return 1;
  }
#endif

static void
add_to_all_sdks (const char *path) {
  all_sdks = insert_each_sdk_subdir (all_sdks, 0, path);
  }

int
main (int argc, char **argv) {
  void (*action) () = do_install;
  int nactions = 0;
  int c;

  set_progname (argv[0]);

  while ((c = getopt_long (argc, argv, shortopts, longopts, NULL)) >= 0)
    switch (c) {
    case 'i':
      nactions++;
      action = do_install;
      alter_toplevel_palmdev = 1;
      break;

    case 'd': {
      static char key[FILENAME_MAX];
      const char *rawkey = skip_sdk_prefix (optarg);

      strcpy (key, rawkey? rawkey : optarg);
      canonicalise_sdk_key (key);
      default_sdk_key = key;
      }
      break;

    case 'R':
      nactions++;	/* **** FIXME */
      action = do_remove;
      break;

    case 'D':
      nactions++;
      action = do_dumpspec;
      dump_target = optarg;
      break;

    case 'r':
      nactions++;
      action = do_remove;
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
  else if (nactions > 1)
    error ("more than one of --install, --dump-specs, --remove given");
  else {
    nextra_trees = argc - optind;
    extra_tree_paths = &argv[optind];

    store = new_string_store ();

    all_sdks = NULL;
    for_each_palmdev_tree (add_to_all_sdks);

    if (default_sdk_key == NULL)
      default_sdk_key = "banana";

    action ();

    free_sdk_list (all_sdks);
    free_string_store (store);
    }

  return (nerrors == 0)? EXIT_SUCCESS : EXIT_FAILURE;
  }
