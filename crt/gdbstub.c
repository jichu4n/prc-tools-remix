/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 *
 *  Modified 19971111 by Ian Goldberg <iang@cs.berkeley.edu>
 *  Modified 19981104 by John Marshall  <jmarshall@acm.org>
 */

#include <SystemMgr.h>
#include <FeatureMgr.h>

#include "NewTypes.h"
#include "crt.h"

int
_GdbStartDebug (UInt16 flags)
{
  UInt32 feature = 0;

  if (! (flags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagSubCall)))
    return 0;

  FtrGet ('gdbS', 0, &feature);
  if (feature != 0x12BEEF34)
    return 0;

  asm("
       lea data_start@END(%%a5),%%a1
       move.l %%a1,%%d2
       lea bss_start@END(%%a5),%%a1
       move.l %%a1,%%d1
       lea start(%%pc),%%a0
       move.l %%a0,%%d0
       sub.l #start, %%d0
       lea PilotMain(%%pc),%%a0
       move.l #0x12BEEF34, %%d3
       trap #8
       " : : : "d0", "d1", "d2", "d3", "a1", "a0");

  return 1;
}
