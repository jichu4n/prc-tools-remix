/* def.l: lexical analyser for .def files.

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

%option nounput

%{
#include <time.h>
#include "pi-dlp.h"

#include "utils.h"
#include "def.tab.h"

static char *dup(const char *s);
%}

%x comment
%x sstr dstr

_	[_\-]?

D	[0-9]
HD	[0-9A-Fa-f]

%%
	char str_buffer[1024];
	char *strp = NULL;

	/* Unsigned numbers: */

0[0-7]*			| 
[1-9][0-9]*		|
0[xX]{HD}+		{ yylval.uint = strtoul(yytext, NULL, 0); return UINT; }

	/* Quoted strings: */

\'			{ strp = str_buffer; BEGIN(sstr); }
\"			{ strp = str_buffer; BEGIN(dstr); }

<sstr>\'		|
<dstr>\"		{ BEGIN(INITIAL); *strp = '\0';
			  yylval.str = dup(str_buffer); return STR; }

<sstr,dstr>\\\n		lineno++;
<sstr,dstr>\n		{ einfo(E_FILELINE | E_WARNING,
				"string constant terminated by end of line");
			  lineno++;
			  BEGIN(INITIAL); *strp = '\0';
			  yylval.str = dup(str_buffer); return STR; }

<sstr,dstr>\\[0-7]{1,3}	*strp++ = (char)strtol(&yytext[1], NULL, 8);

<sstr,dstr>\\x{HD}{HD}	*strp++ = (char)strtol(&yytext[2], NULL, 16);

<sstr,dstr>\\n		*strp++ = '\n';
<sstr,dstr>\\t		*strp++ = '\t';
<sstr,dstr>\\r		*strp++ = '\r';
<sstr,dstr>\\b		*strp++ = '\b';
<sstr,dstr>\\f		*strp++ = '\f';

<sstr,dstr>\\.		*strp++ = yytext[1];

<sstr>[^\\\n\']+	|
<dstr>[^\\\n\"]+	{ strcpy(strp, yytext); strp += yyleng; }

	/* Comments and whitespace: */

"/*"			BEGIN(comment);
<comment>[^*\n]*	/* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*	/* eat up '*'s not followed by '/'s */
<comment>\n		lineno++;
<comment>"*"+"/"	BEGIN(INITIAL);

"//"[^\n]*		/* eat C++-style comments */
[ \t]+			/* eat whitespace */
\n			lineno++;

	/* Reserved words: */

	/* These have yylvals for convenience, and are separate terminals
	   in case the grammar wants to treat them differently.  */
(app)|(application)	{ yylval.kind = DK_APPLICATION; return APPLICATION; }
glib			{ yylval.kind = DK_GLIB; return GLIB; }
syslib			{ yylval.kind = DK_SYSLIB; return SYSLIB; }
hack			{ yylval.kind = DK_HACK; return HACK; }
database		{ yylval.kind = DK_GENERIC; return DATABASE; }

code			return CODE;
data			return DATA;

trap			return TRAP;
multiple		return MULTIPLE;
export			return EXPORT;

type			return TYPE;
version			return VERSION;
(modno)|(modification)	return MODNO;

read{_}only		{ yylval.flag = dlpDBFlagReadOnly; return DBFLAG; }
appinfo{_}dirty		{ yylval.flag = dlpDBFlagAppInfoDirty; return DBFLAG; }
backup			{ yylval.flag = dlpDBFlagBackup; return DBFLAG; }
ok{_}to{_}install{_}newer { yylval.flag = dlpDBFlagNewer; return DBFLAG; }
reset{_}after{_}install	{ yylval.flag = dlpDBFlagReset; return DBFLAG; }
copy{_}prevention	{ yylval.flag = dlpDBFlagCopyPrevention; return DBFLAG;}
stream			{ yylval.flag = dlpDBFlagStream; return DBFLAG; }

	/* Other words ("identifiers", but for us they're unquoted strings): */

[A-Za-z_][A-Za-z0-9_]*	{ yylval.str = dup(yytext); return STR; }

<*>.	einfo(E_FILELINE | E_WARNING, "unexpected character `%c'", yytext[0]);

%%

int
yywrap() {
  return 1;
  }

extern struct string_store *lexer_store;

static char *
dup(const char *s) {
  return insert_string(lexer_store, s);
  }
