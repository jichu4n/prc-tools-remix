/* genbootstrap.c: generate bootstrap headers for compiling startup code.

   Copyright 2001 by John Marshall.
   <jmarshall@acm.org>

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include <SystemMgr.h>
#include <FileStream.h>
#include <FeatureMgr.h>
#include <StringMgr.h>

#include <NewTypes.h>
#include <stdarg.h>

#include "genbootstrap.h"

static Boolean on_poser;

void
console (const char *text) {
  static int y = 0;

  while (1) {
    const char *cr = StrChr (text, '\n');
    WinDrawChars (text, cr? cr - text : StrLen (text), 0, y);
    if (!cr)
      break;
    y += 11;
    text = cr + 1;
    }
  }


static FileHand outf;

void
filestream (const char *text) {
  Err err;
  FileWrite (outf, text, 1, StrLen (text), &err);
  }


static void
vfprintf (void (*output) (const char *), const char *format, va_list args) {
  static char buffer[2048];  /* static just to keep it off the stack.  */
  StrVPrintF (buffer, format, args);
  output (buffer);
  }

void
printf (const char *format, ...) {
  va_list args;
  va_start (args, format);
  vfprintf (filestream, format, args);
  va_end (args);
  }

void
fprintf (void (*output) (const char *), const char *format, ...) {
  va_list args;
  va_start (args, format);
  vfprintf (output, format, args);
  va_end (args);
  }


static void
generate (const char *fname, void (*generator) (const char *)) {
  Err err = 0;

  outf = FileOpen (0, fname, 0, 0, fileModeReadWrite, &err);
  if (outf && err == 0) {
    fprintf (console, "Generating filestream %s\n", fname);
    generator (fname);
    FileClose (outf);

    if (on_poser) {
      fprintf (console, "Comparing to host file...\n");
      copy_if_different (fname);
      }
    }
  else
    fprintf (console, "* Can't create filestream %s\n", fname);
  }


#if SDK_VERSION >= 35
typedef void cmdPBP_base;
#else
typedef char cmdPBP_base;
#endif

#define UNUSED __attribute__ ((unused))

UInt32
PilotMain (UInt16 cmd, cmdPBP_base *cmdPBP UNUSED, UInt16 launchFlags UNUSED) {
  if (cmd == sysAppLaunchCmdNormalLaunch) {
    UInt32 value;

    on_poser = FtrGet ('pose', 0, &value) == 0;

    generate ("bootstrap.h", bootstrap);
    generate ("bootstrap-ui.h", bootstrap_ui);

    fprintf (console, "...done\n");
    SysTaskDelay (2 * SysTicksPerSecond ());
    }

  return 0;
  }
