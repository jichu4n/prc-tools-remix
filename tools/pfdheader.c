/* pfdheader.c: simple database header utility functions.

   Copyright (c) 1999, 2001 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <string.h>

#include "pfdheader.h"

void
init_database_header (struct database_header *h) {
  strncpy (h->name, "", 32);
  strncpy (h->type, "", 4);
  strncpy (h->creator, "", 4);

  h->readonly = h->appinfo_dirty = h->backup = 0;
  h->ok_to_install_newer = h->reset_after_install = 0;
  h->copy_prevention = h->stream = 0;
  h->hidden = h->launchable_data = 0;
  h->recyclable = h->bundle = 0;

  h->version = h->modnum = h->uidseed = 0;

  tm_of_palmostime (&h->created, 0);
  tm_of_palmostime (&h->modified, 0);
  tm_of_palmostime (&h->backedup, 0);
  }
