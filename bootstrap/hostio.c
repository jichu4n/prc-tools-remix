/* hostio.c: copy a File Stream to a host file.

   Copyright 2001 John Marshall.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <MemoryMgr.h>
#include <SystemMgr.h>
#include <FileStream.h>
#include <HostControl.h>

#include "genbootstrap.h"

static int
identical (FileHand f1, HostFILE *f2) {
  static char buf1[1024], buf2[1024];
  Int32 n1, n2;
  Err err = 0;

  while (1) {
    n1 = FileRead (f1, buf1, 1, sizeof buf1, &err);
    if (err != 0 && err != fileErrEOF)  n1 = -1;

    n2 = HostFRead (buf2, 1, sizeof buf2, f2);

    if (n1 <= 0 || n2 <= 0)
      break;

    if (n1 != n2 || MemCmp (buf1, buf2, n1) != 0)
      return 0;
    }

  return n1 == 0 && n2 == 0;
  }

void
copy_if_different (const char *fname) {
  Err err = 0;
  FileHand srcf = FileOpen (0, fname, 0, 0, fileModeReadOnly, &err);

  if (srcf && err == 0) {
    int copy_needed;
    HostFILE *dstf = HostFOpen (fname, "r");

    if (dstf) {
      copy_needed = ! identical (srcf, dstf);
      HostFClose (dstf);
      }
    else
      copy_needed = 1;

    if (copy_needed) {
      fprintf (console, "%s host file %s\n",
	       dstf? "Updating" : "Creating", fname);

      dstf = HostFOpen (fname, "w");
      if (dstf) {
	static char buffer[2048];
	Int32 n;

	FileRewind (srcf);

	while ((n = FileRead (srcf, buffer, 1, sizeof buffer, &err)) > 0
	       && (err == 0 || err == fileErrEOF))
	  HostFWrite (buffer, 1, n, dstf);

	HostFClose (dstf);
	}
      else
	fprintf (console, "* Can't write to host %s\n", fname);
      }
    else
      fprintf (console, "%s is unchanged\n", fname);

    FileClose (srcf);
    }
  else
    fprintf (console, "* Reopening %s failed!\n", fname);
  }
