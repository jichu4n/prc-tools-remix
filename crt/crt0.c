/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 *
 *  Modified 19971111 by Ian Goldberg <iang@cs.berkeley.edu>
 *  Modified 19981104 by John Marshall  <jmarshall@acm.org>
 */

#include <Common.h>
#include <System/SysAll.h>
#define NON_PORTABLE
#include <SystemPrv.h>
#include <FeatureMgr.h>

#include "crt.h"

ULong start ()
{
  SysAppInfoPtr appInfo;
  Ptr prevGlobals;
  Ptr globalsPtr;
  ULong result;
  Word mainCmd;
  Ptr mainPBP;
  Word mainFlags;

  if (SysAppStartup (&appInfo, &prevGlobals, &globalsPtr) != 0)
    {
      SndPlaySystemSound (sndError);
      return -1;
    }

  mainCmd = appInfo->cmd;
  mainPBP = appInfo->cmdPBP;
  mainFlags = appInfo->launchFlags;

  if (mainFlags & sysAppLaunchFlagNewGlobals)
    _GccRelocateData (); 

  __do_bhook (mainCmd, mainPBP, mainFlags);

  if (mainFlags & sysAppLaunchFlagNewGlobals)
    __do_ctors ();

#ifdef DASHG
  _GdbStartDebug (mainFlags);
#endif

  result = PilotMain (mainCmd, mainPBP, mainFlags);

  if (mainFlags & sysAppLaunchFlagNewGlobals)
    __do_dtors ();

  __do_ehook (mainCmd, mainPBP, mainFlags);

  SysAppExit (appInfo, prevGlobals, globalsPtr);

  return result;
}
