/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 *
 *  Modified 19971111 by Ian Goldberg <iang@cs.berkeley.edu>
 *  Modified 19981104 by John Marshall  <jmarshall@acm.org>
 *  Modified 20000425 by John Marshall  <john_w_marshall@palm.com>
 */

#include <stddef.h>
#ifdef BOOTSTRAP
#include "bootstrap.h"
#else
#include <SystemMgr.h>
#include <MemoryMgr.h>
#include <DataMgr.h>
#include "NewTypes.h"
#endif

#include "crt.h"

extern UInt32 start ();

#ifdef Lsingle_dreloc

void
_GccRelocateData ()
{
  MemHandle relocH = DmGet1Resource ('rloc', 0);
  if (relocH)
    {
      Int16 *chain = MemHandleLock (relocH);

      _RelocateChain (*chain++, &data_start);
      _RelocateChain (*chain++, (void *) &start);

      MemHandleUnlock (relocH);
      DmReleaseResource (relocH);
    }
}

#endif
#ifdef Lmulti_dreloc

extern void *__text__;

void
_GccLoadCodeAndRelocateData ()
{
  void **basep = &__text__;
  MemHandle codeH, relocH;
  int resno;

  *basep++ = (void *) &start;
  for (resno = 2; (codeH = DmGet1Resource ('code', resno)) != NULL; resno++)
    *basep++ = MemHandleLock (codeH);

  if ((relocH = DmGet1Resource ('rloc', 0)) != NULL) 
    {
      Int16 *chain = MemHandleLock (relocH);
      void **baselim = basep;

      _RelocateChain (*chain++, &data_start);
      for (basep = &__text__; basep < baselim; basep++)
	_RelocateChain (*chain++, *basep);

      MemHandleUnlock (relocH);
      DmReleaseResource (relocH);
    }
}

#endif
#ifdef Lmulti_free

void
_GccReleaseCode (UInt16 cmd UNUSED_PARAM, void *pbp UNUSED_PARAM, UInt16 flags)
{
  if (flags & sysAppLaunchFlagNewGlobals)
    {
      MemHandle codeH;
      int resno;

      for (resno = 2; (codeH = DmGet1Resource ('code', resno)) != NULL; resno++)
	{
	  MemHandleUnlock (codeH);
	  DmReleaseResource (codeH);
	}
    }
}

#endif
#ifdef Lreloc_chain

union reloc
{
  struct { Int16 next; UInt16 addend; } r;
  UInt32 value;
};

void
_RelocateChain (Int16 offset, void *base)
{
  char *data_res = &data_start;

  while (offset >= 0)
    {
      union reloc *site = (union reloc *) (data_res + offset);
      offset = site->r.next;
      site->r.next = 0;
      site->value += (UInt32) base;
    }
}

#endif
