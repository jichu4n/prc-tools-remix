/* genbootstrap.h: generate bootstrap headers for compiling startup code.

   Copyright 2001 John Marshall.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#ifndef GENBOOTSTRAP_H
#define GENBOOTSTRAP_H

void console (const char *);
void filestream (const char *);

void printf (const char *format, ...);
void fprintf (void (*output) (const char *), const char *format, ...);

void copy_if_different (const char *fname);

void bootstrap (const char *);
void bootstrap_ui (const char *);

#endif
