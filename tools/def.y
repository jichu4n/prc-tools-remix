/* def.y: parser for .def files.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.

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
#include <stdlib.h>
#include <time.h>

#include "pi-dlp.h"
#include "pi-source.h"

#include "utils.h"
#include "def.h"
#include "config.h"

static struct def_database_info ginfo;
static const struct def_callbacks *gcallbacks;

void yyerror(char *s);
int yylex();

static unsigned long lookup_trapno(const char *name);
%}

%union {
  unsigned long uint;
  char *str;
  unsigned short flag;
  enum database_kind kind;
  }

%token	MULTIPLE CODE DATA TRAP EXPORT
%token	TYPE VERSION MODNO

%token	<uint>	UINT
%token	<str>	STR
%token	<kind>	APPLICATION GLIB SYSLIB HACK DATABASE
%token	<flag>	DBFLAG

%type	<kind>	database_kind
%type	<uint>	trap_w_section trap trap_spec
%type	<str>	section_spec section

%%

def_file:
	  database_def def_list
	;

database_def:
	  database_kind '{' STR STR header_item_list '}'
	  	{ ginfo.kind = $1;
		  strcpy(ginfo.db.name, $3);
		  ginfo.db.creator = $4;
		  }
	;

database_kind:
	  APPLICATION
	| GLIB
	| SYSLIB
	| HACK
	| DATABASE
	;
	
header_item_list:
	  /* empty */
	| header_item_list header_item
	;

header_item:
	  TYPE '=' STR
		{ ginfo.db.type = $3; ginfo.type_specified = 1; }
	| VERSION '=' UINT	{ ginfo.db.version = $3; }
	| MODNO '=' UINT	{ ginfo.db.modnum = $3; }
	| DBFLAG		{ ginfo.db.flags |= $1; }
	;

def_list:
	  /* empty */
	| def_list def
	;

def:
	  trap_def
	| multiple_code_def
	| export_def
	;

trap_def:
	  TRAP '{' trap_list '}'

trap_list:
	  /* empty */
	| trap_list trap_w_section
	;

trap_w_section:
	  trap
	| trap '{' section_spec '}'
	;

trap:
	  trap_spec
	| trap_spec '(' UINT ')'
	;

trap_spec:
	  UINT		{ $$ = $1; }
	| STR		{ $$ = lookup_trapno($1); }
	;

multiple_code_def:
	  MULTIPLE CODE '{' section_list '}'
	;

section_list:
	  /* empty */
	| section_list section
	;

section:
	  section_spec
	| section_spec '(' UINT ')'
	;

section_spec:
	  STR
	| '<' STR '>'			{ $$ = $2; }
	| '<' STR '.' STR '>'		{ $$ = $2; }
	;


export_def:
	  EXPORT '{' '}'
	;

%%

void
yyerror(char *s) {
  }


struct string_store *lexer_store = NULL;

static char *trapno_text = NULL;

static unsigned long
lookup_trapno(const char *name) {
  static int trapfile_missing = 0;

  char *record;
  int namelen;

  if (trapno_text == NULL && !trapfile_missing) {
    long length;
    if ((trapno_text = slurp_file(TRAPNO_FNAME, "r", &length)) == NULL) {
      einfo(E_NOFILE | E_PERROR, "can't open config file `%s'", TRAPNO_FNAME);
      trapfile_missing = 1;
      }
    }

  if (trapfile_missing)
    return 0;

  namelen = strlen(name);
  for (record = trapno_text+1; (record = strstr(record, name)); record++)
    if (record[-1] == '<' && record[namelen] == '>')
      return strtoul(&record[namelen+1], NULL, 0);

  einfo(E_FILELINE, "unknown systrap `%s'", name);
  return 0;
  }


static void
cleanup() {
  free_string_store(lexer_store);
  free(trapno_text);
  }

int
read_def_file(const char *fname, struct def_database_info *info,
	      const struct def_callbacks *callbacks) {
  if (lexer_store == NULL) {
    lexer_store = new_string_store();
    atexit(cleanup);
    }

  filename = fname;
  lineno = 1;

  gcallbacks = callbacks;

  ginfo.db.flags = 0;
  ginfo.db.version = 0;
  ginfo.db.modnum = 0;
  strcpy(ginfo.db.name, "");
  ginfo.type_specified = 0;

  yyparse();

  if (!ginfo.type_specified && ginfo.kind != DK_GENERIC) {
    ginfo.db.type = standard_db_type(ginfo.kind);
    ginfo.type_specified = 1;
    }

  if (info)
    *info = ginfo;

  return 0;
  }
