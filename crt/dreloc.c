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
#include <MemoryMgr.h>
#include <DataMgr.h>

#include "sdktypes.h"
#include "crt.h"

extern char data_start;

#ifdef Lsingle_dreloc

void
_GccRelocateData ()
{
  MemHandle relocH = DmGet1Resource ('rloc', 0);
  if (relocH)
    {
      Int16 *chain = MemHandleLock (relocH);

      _RelocateChain (*chain++, &data_start);
      _RelocateChain (*chain++, &start);

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
  MemHandle relocH = DmGet1Resource ('rloc', 0);
  Int16 *chain = (relocH)? MemHandleLock (relocH) : NULL;
  void **basep = &__text__;
  int resno, ncoderes;

  *basep = &start;

  if (chain)
    {
      _RelocateChain (*chain++, &data_start);
      _RelocateChain (*chain++, &start);
    }

  asm ("move.w #__code_section_count,%0" : "=g" (ncoderes) : : "cc");

  for (resno = 2; resno <= ncoderes; resno++)
    {
      MemHandle codeH = DmGet1Resource ('code', resno);
      *++basep = MemHandleLock (codeH);
      if (chain)
	_RelocateChain (*chain++, *basep);
    }

  if (relocH)
    {
      MemHandleUnlock (relocH);
      DmReleaseResource (relocH);
    }
}

#endif
#ifdef Lmulti_free

extern void *__text__;

void
_GccReleaseCode (UInt16 cmd UNUSED_PARAM, void *pbp UNUSED_PARAM, UInt16 flags)
{
  if (flags & sysAppLaunchFlagNewGlobals)
    {
      void **basep = &__text__;
      int ncoderes;

      asm ("move.w #__code_section_count,%0" : "=g" (ncoderes) : : "cc");

      while (--ncoderes > 0)
	{
	  MemHandle hnd = MemPtrRecoverHandle (*++basep);
	  MemHandleUnlock (hnd);
	  DmReleaseResource (hnd);
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
