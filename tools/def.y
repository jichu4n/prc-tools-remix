/* def.y: parser for .def files.

   Copyright (c) 1999, 2001 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program follows in the footsteps of obj-res and build-prc, the
   source code of which contains the following notices:

 * obj-res.c:  Dump out .prc compatible binary resource files from an object
 *
 * (c) 1996, 1997 Dionne & Associates
 * jeff@ryeham.ee.ryerson.ca
 *
 * This is Free Software, under the GNU Public Licence v2 or greater.
 *
 * Relocation added March 1997, Kresten Krab Thorup 
 * krab@california.daimi.aau.dk

 * ptst.c:  build a .prc from a pile of files.
 *
 * (c) 1996, Dionne & Associates
 * (c) 1997, The Silver Hammer Group Ltd.
 * This is Free Software, under the GNU Public Licence v2 or greater.
 */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "def.h"
#include "pfdheader.h"

#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))
#else
#define UNUSED_PARAM
#endif

static const struct def_callbacks *call;
static struct database_header db;
static const char *parser_fname;

extern int yylex ();
extern FILE *yyin;

extern void lexer_init (void (*warning_func) (const char *));
extern int lexer_lineno ();

void yyerror (char *s);

static unsigned long lookup_trapno (const char *name);
%}

%union {
  unsigned long uint;
  char *str;
  enum database_kind kind;
  struct { unsigned int vector; char *fname; } trap;
  }

%token	MULTIPLE CODE DATA TRAP EXPORT STACK
%token	VERSION MODNO
%token	READONLY APPINFO_DIRTY BACKUP OK_TO_INSTALL_NEWER RESET_AFTER_INSTALL
%token	COPY_PREVENTION STREAM HIDDEN LAUNCHABLE_DATA

%token	<uint>	UINT
%token	<str>	STR FNAME
%token	<kind>	APPLICATION GLIB SYSLIB HACK DATABASE

%type	<kind>	project_kind specific_db_kind
%type	<uint>	trap_vector trap_list resource_number
%type	<str>	str_or_file
%type	<trap>	trap

%%

def_file:
	  database_def def_list
	;

database_def:
	  project_kind '{' STR STR header_item_list '}'
				{ strncpy (db.name, $3, 32);
				  strncpy (db.creator, $4, 4);
				  call->db_header ($1, &db); }
	;

project_kind:
	  specific_db_kind	{ strncpy (db.type, standard_db_type ($1), 4);
				  $$ = $1; }
	| specific_db_kind STR	{ strncpy (db.type, $2, 4); $$ = $1; }
	| DATABASE STR		{ strncpy (db.type, $2, 4); $$ = DK_GENERIC; }
	;


specific_db_kind:
	  APPLICATION
	| GLIB
	| SYSLIB
	| HACK
	;
	
header_item_list:
	  /* empty */
	| header_item_list header_item
	;

header_item:
	  VERSION '=' UINT	{ db.version = $3; }
	| MODNO '=' UINT	{ db.modnum = $3; }
	| READONLY		{ db.readonly = 1; }
	| APPINFO_DIRTY		{ db.appinfo_dirty = 1; }
	| BACKUP		{ db.backup = 1; }
	| OK_TO_INSTALL_NEWER	{ db.ok_to_install_newer = 1; }
	| RESET_AFTER_INSTALL	{ db.reset_after_install = 1; }
	| COPY_PREVENTION	{ db.copy_prevention = 1; }
	| STREAM		{ db.stream = 1; }
	| HIDDEN		{ db.hidden = 1; }
	| LAUNCHABLE_DATA	{ db.launchable_data = 1; }
	| DATA			{ call->data (1); }
	| STACK '=' UINT	{ call->stack ($3); }
	;

def_list:
	  /* empty */
	| def_list def
	;

def:
	  trap_def
	| version_def
	| multiple_code_def
	| export_def
	;

trap_def:
	  TRAP '{' trap_list '}'

trap_list:
	  /* empty */		{ $$ = 1000; }
	| trap_list trap	{ call->trap ($1, $2.vector, $2.fname);
				  $$ = $1 + 1; }
	;

trap:
	  trap_vector	{ $$.vector = $1; $$.fname = NULL; }
	| trap_vector FNAME
			{ $$.vector = $1; $$.fname = $2; }
	;

trap_vector:
	  UINT		{ $$ = $1; }
	| STR		{ $$ = lookup_trapno ($1); }
	;

version_def:
	  VERSION resource_number str_or_file
			{ call->version_resource ($2, $3); }
	;

resource_number:
	  /* empty */	{ $$ = 1; }
	| UINT		{ $$ = $1; }
	;

str_or_file:
	  STR		{ $$ = $1; }
	| FNAME		{ long len;
			  char *text = slurp_file ($1, "r", &len);
			  if (text) { chomp (text); $$ = text; }
			  else { error ("[%s:%d] can't read '%s': @P",
					parser_fname, lexer_lineno (), $1);
				 $$ = $1; } }
	;

multiple_code_def:
	  MULTIPLE CODE '{' sectionname_list '}'
	;

sectionname_list:
	  /* empty */
	| sectionname_list STR	{ call->multicode_section ($2); }
	;

export_def:
	  EXPORT '{' export_list '}'
	;

export_list:
	  /* empty */
	| export_list STR	{ call->export_function ($2); }
	;

%%

void
yyerror (char *message) {
  error ("[%s:%d] %s", parser_fname, lexer_lineno (), message);
  }

static void
print_warning (const char *message) {
  warning ("[%s:%d] %s", parser_fname, lexer_lineno (), message);
  }


struct string_store *lexer_store = NULL;

static char *trapno_text = NULL;

static unsigned long
lookup_trapno (const char *name) {
  static int trapfile_missing = 0;

  char *record;
  int namelen;

#if 0
/* The configury involved in finding the TRAPNO_FNAME has changed
   significantly.  We'll reenable this stuff and do it right when we
   implement HACK support in .def files, i.e., when it'll be useful.  */
  if (trapno_text == NULL && !trapfile_missing) {
    long length;
    if ((trapno_text = slurp_file (TRAPNO_FNAME, "r", &length)) == NULL) {
      error ("can't open config file '%s': @P", TRAPNO_FNAME);
      trapfile_missing = 1;
      }
    }
#endif

  if (trapfile_missing)
    return 0;

  namelen = strlen (name);
  for (record = trapno_text+1; (record = strstr (record, name)); record++)
    if (record[-1] == '<' && record[namelen] == '>')
      return strtoul (&record[namelen+1], NULL, 0);

  error ("[%s:%d] unknown systrap '%s'", parser_fname, lexer_lineno (), name);
  return 0;
  }


static void
cleanup () {
  free_string_store (lexer_store);
  free (trapno_text);
  }


void
read_def_file (const char *fname, const struct def_callbacks *callbacks) {
  if (lexer_store == NULL) {
    lexer_store = new_string_store ();
    atexit (cleanup);
    }

  call = callbacks;
  parser_fname = fname;

  lexer_init (print_warning);
  init_database_header (&db);

  if ((yyin = fopen (parser_fname, "r")) != NULL) {
    yyparse ();
    fclose (yyin);
    }
  else
    error ("can't open def file '%s': @P", parser_fname);
  }

static void default_i (int i UNUSED_PARAM) {}
static void default_str (const char *s UNUSED_PARAM) {}
static void default_ul (unsigned long ul UNUSED_PARAM) {}
static void default_ui_ui_str (unsigned int ui1 UNUSED_PARAM,
			       unsigned int ui2 UNUSED_PARAM,
			       const char *s UNUSED_PARAM) {}
static void default_kind_dh (enum database_kind k UNUSED_PARAM,
			     const struct database_header *h UNUSED_PARAM) {}
static void default_ul_str (unsigned long ul UNUSED_PARAM,
			    const char *s UNUSED_PARAM) {}

struct def_callbacks default_def_callbacks = {
  default_kind_dh,
  default_str,
  default_str,
  default_ui_ui_str,
  default_i,
  default_ul,
  default_ul_str
  };
