/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 *
 *  Last modified 19971111 by Ian Goldberg <iang@cs.berkeley.edu>
 */

#include <Common.h>
#include <System/SysAll.h>
#define NON_PORTABLE
#include <SystemPrv.h>
#include <FeatureMgr.h>

static void GccRelocateData(void);
static void do_bhook(Word,Ptr,Word);
static void do_ehook(Word,Ptr,Word);
static void do_ctors(void);
static void do_dtors(void);
#ifdef DASHG
int GdbStartDebug(void);
#endif

register ULong reg_a4 asm("%a4");

ULong start ()
{
  SysAppInfoPtr appInfo;
  Ptr prevGlobals;
  Ptr globalsPtr;
  ULong save_a4, result;
  int did_ctors = 0;

  save_a4 = reg_a4;
  
  if (SysAppStartup (&appInfo, &prevGlobals, &globalsPtr) != 0)
    {
      SndPlaySystemSound (sndError);
      reg_a4 = save_a4;
      return -1;
    }
  else
    {
	Word mainCmd = appInfo->cmd;
	Ptr mainPBP = appInfo->cmdPBP;
	Word mainFlags = appInfo->launchFlags;

      if (mainFlags & (sysAppLaunchFlagNewGlobals|sysAppLaunchFlagSubCall))
	{
	  asm volatile ("move.l %a5,%a4; sub.l #edata,%a4");
	} else {
	  reg_a4 = 0;
	}
      if (mainFlags & sysAppLaunchFlagNewGlobals) {
	  GccRelocateData (); 
          do_ctors();
          did_ctors = 1;
      }

      do_bhook(mainCmd, mainPBP, mainFlags);
#ifdef DASHG
      GdbStartDebug();
#endif
      result = PilotMain (mainCmd, mainPBP, mainFlags);
      do_ehook(mainCmd, mainPBP, mainFlags);
      if (did_ctors) {
          do_dtors();
      }
      SysAppExit (appInfo, prevGlobals, globalsPtr);

      reg_a4 = save_a4;
      return result;
    }
}

struct pilot_reloc {
  UChar  type;
  UChar  section;  
  UInt   offset;
  ULong  value ;
};

#define TEXT_SECTION 't'
#define DATA_SECTION 'd'
#define BSS_SECTION  'b'

#define RELOC_ABS_32       0xbe

#if 0
union reloc
{
  struct { UInt next, offset; } r;
  ULong value;
};

static void GccRelocateData2 (char *datares, ULong base, UInt loc_offset)
{
  while (loc_offset != 0xffff)
    {
      union reloc *reloc = (union reloc *)&datares[loc_offset];
      loc_offset = reloc->r.next;
      reloc->value = base + reloc->r.offset;
    }
}

static void GccRelocateData ()
{
  VoidHand relocH;

  relocH = DmGet1Resource ('rloc', 0);
  if (relocH == 0)
    return;

  relocPtr = MemHandleLock (relocH);

  MemHandleUnlock (relocH);
  DmReleaseResource (relocH);
}
#endif

/*
 *  This function should be called from 
 */
static void GccRelocateData ()
{
  extern long data_start, bss_start;
  unsigned long data = (unsigned long)&data_start;
  unsigned long bss  = (unsigned long)&bss_start;
  unsigned long text = (unsigned long)&start;

  VoidHand relocH;
  char *relocPtr;
  struct pilot_reloc *relocs;
  UInt count, i;

  static int done = 0;
  
  if (done) return;
  else done = 1;

  asm ("sub.l #start, %0" : "=g" (text) : "0" (text));
  asm ("sub.l #bss_start, %0" : "=g" (bss) : "0" (bss));
  asm ("sub.l #data_start, %0" : "=g" (data) : "0" (data));
  
  relocH = DmGet1Resource ('rloc', 0);
  if (relocH == 0)
    return;

  relocPtr = MemHandleLock (relocH);
  count = *(UInt*)relocPtr;
  relocs = (struct pilot_reloc*) (relocPtr + 2);

  for (i = 0; i < count; i++)
    {
      unsigned long *loc;
      ErrFatalDisplayIf (relocs[i].type != RELOC_ABS_32, \
			 "unknown reloc.type");

      loc = (unsigned long*) ((char*)&data_start + relocs[i].offset);

      switch (relocs[i].section)
	{
	case TEXT_SECTION:
	  *loc += text;
	  break;

	case DATA_SECTION:
	  *loc += data;
	  break;

	case BSS_SECTION:
	  *loc += bss;
	  break;
	  
	default:
	  ErrDisplay ("Unknown reloc.section");
	}
    }

  MemHandleUnlock (relocH);
  DmReleaseResource (relocH);

}

static void do_bhook(Word cmd, Ptr PBP, Word flags)
{
    void **hookend, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea bhook_start,%0; add.l %1,%0" : "=a" (hookptr) : "g" (text));
    asm ("lea bhook_end,%0; add.l %1,%0" : "=a" (hookend) : "g" (text));

    while (hookptr < hookend) {
	void (*fptr)(Word,Ptr,Word) = (*(hookptr++)) + text;
	fptr(cmd,PBP,flags);
    }
}

static void do_ehook(Word cmd, Ptr PBP, Word flags)
{
    void **hookstart, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea ehook_start,%0; add.l %1,%0" : "=a" (hookstart) : "g" (text));
    asm ("lea ehook_end,%0; add.l %1,%0" : "=a" (hookptr) : "g" (text));

    while (hookptr > hookstart) {
	void (*fptr)(Word,Ptr,Word) = (*(--hookptr)) + text;
	fptr(cmd,PBP,flags);
    }
}

static void do_ctors(void)
{
    void **hookend, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea ctors_start,%0; add.l %1,%0" : "=a" (hookptr) : "g" (text));
    asm ("lea ctors_end,%0; add.l %1,%0" : "=a" (hookend) : "g" (text));

    while (hookptr < hookend) {
	void (*fptr)(void) = (*(hookptr++)) + text;
	fptr();
    }
}

static void do_dtors(void)
{
    void **hookstart, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea dtors_start,%0; add.l %1,%0" : "=a" (hookstart) : "g" (text));
    asm ("lea dtors_end,%0; add.l %1,%0" : "=a" (hookptr) : "g" (text));

    while (hookptr > hookstart) {
	void (*fptr)(void) = (*(--hookptr)) + text;
	fptr();
    }
}

#ifdef DASHG
int GdbStartDebug(void)
{
  DWord feature = 0;

  if (!reg_a4) return 0;

  FtrGet('gdbS', 0, &feature);
  if (feature != 0x12BEEF34) return 0;

  asm("
       lea data_start(%%a4),%%a1
       move.l %%a1,%%d2
       lea bss_start(%%a4),%%a1
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
#endif
