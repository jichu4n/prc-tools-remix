/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 *
 *  Modified 19971111 by Ian Goldberg <iang@cs.berkeley.edu>
 *  Modified 19981104 by John Marshall  <jmarshall@acm.org>
 */

#ifdef BOOTSTRAP
#include "bootstrap.h"
#else
#include <SystemMgr.h>
#include <FeatureMgr.h>
#include "NewTypes.h"
#endif

#include "crt.h"

static void
StartDebug (UInt16 cmd UNUSED_PARAM, void *pbp UNUSED_PARAM, UInt16 flags)
{
  UInt32 feature = 0;

  if (! (flags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagSubCall)))
    return;

  FtrGet ('gdbS', 0, &feature);
  if (feature != 0x12BEEF34)
    return;

  asm("
       lea %0,%%a1
       move.l %%a1,%%d2
       lea %1,%%a1
       move.l %%a1,%%d1
       lea start(%%pc),%%a0
       move.l %%a0,%%d0
       sub.l #start, %%d0
       lea PilotMain(%%pc),%%a0
       move.l #0x12BEEF34, %%d3
       trap #8
       " : : "g" (data_start), "g" (bss_start)
         : "d0", "d1", "d2", "d3", "a1", "a0");
}

static void *hook __attribute__ ((section ("bhook"), unused)) = StartDebug;
