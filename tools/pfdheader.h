#ifndef PFDHEADER_H
#define PFDHEADER_H

#include <time.h>

struct database_header {
  /* These are fixed length arrays, and should be converted to/from strings
     and compared like this:
	strncpy (db.type, "appl", 4);
	if (strncmp (db.type, "DATA", 4) == 0) { ... }
	printf ("type: '%.4s'", db.type);
     Add "sizeof db.type" and "%.*s" to taste...  */

  char name[32], type[4], creator[4];

  /* These are booleans: values are zero and non-zero.  */

  int readonly, appinfo_dirty, backup, ok_to_install_newer,
      reset_after_install, copy_prevention, stream, hidden,
      launchable_data, recyclable, bundle;

  /* These really are unsigned longs, though uidseed must be < 2^24.  */

  unsigned long version, modnum, uidseed;

  /* The Palm OS dates these represent may be "undefined".  You can detect
     this by comparing against tm_of_palmostime (0).  */

  struct tm created, modified, backedup;
  };


/* Take yer pick: a c_style_name or a CPlusPlusStyle one.  */

typedef struct database_header DatabaseHeader;


#ifdef __cplusplus
extern "C" {
#endif

void init_database_header (struct database_header *h);

unsigned long palmostime_of_tm (const struct tm *tm);
void tm_of_palmostime (struct tm *tm, unsigned long secs);

#ifdef __cplusplus
}
#endif

#endif
