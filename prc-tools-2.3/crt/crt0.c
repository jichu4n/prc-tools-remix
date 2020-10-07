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

#ifdef __OWNGP__
register UInt32 reg_a4 asm ("%a4");
#endif

UInt32
start ()
{
  SysAppInfoType *appInfo;
  void *prevGlobals;
  void *globalsPtr;

  if (SysAppStartup (&appInfo, &prevGlobals, &globalsPtr) != 0)
    {
      SndPlaySystemSound (sndError);
      return -1;
    }
  else
    {
      Int16 mainCmd = appInfo->cmd;
      void *mainPBP = appInfo->cmdPBP;
      UInt16 mainFlags = appInfo->launchFlags;
      UInt32 result;

#ifdef __OWNGP__
      UInt32 save_a4 = reg_a4;

      if (mainFlags & sysAppLaunchFlagNewGlobals)
	asm ("move.l %%a5,%%a4; sub.l #edata,%%a4" : : : "%a4");
      else
	reg_a4 = 0;
#endif

      if (mainFlags & sysAppLaunchFlagNewGlobals)
	_GccRelocateData ();

      __do_bhook (mainCmd, mainPBP, mainFlags);

      if (mainFlags & sysAppLaunchFlagNewGlobals)
	__do_ctors ();

      result = PilotMain (mainCmd, mainPBP, mainFlags);

      if (mainFlags & sysAppLaunchFlagNewGlobals)
	__do_dtors ();

      __do_ehook (mainCmd, mainPBP, mainFlags);

#ifdef __OWNGP__
      reg_a4 = save_a4;
#endif

      SysAppExit (appInfo, prevGlobals, globalsPtr);

      return result;
    }
}
