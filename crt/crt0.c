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
#include <SoundMgr.h>

#include "NewTypes.h"

#if SDK_VERSION < 35
#define NON_PORTABLE
#include <SystemPrv.h>
#endif

#endif

#include "crt.h"

UInt32
start ()
{
  SysAppInfoType* appInfo;
  void *prevGlobals;
  void *globalsPtr;
  UInt32 result;
  Int16 mainCmd;
  void *mainPBP;
  UInt16 mainFlags;

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

  result = PilotMain (mainCmd, mainPBP, mainFlags);

  if (mainFlags & sysAppLaunchFlagNewGlobals)
    __do_dtors ();

  __do_ehook (mainCmd, mainPBP, mainFlags);

  SysAppExit (appInfo, prevGlobals, globalsPtr);

  return result;
}
