/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 */

#pragma pack(2)

#define NON_PORTABLE

#include <Common.h>
#include <System/Globals.h>
#include <UI/ScrDriver.h>
#include <System/SysAll.h>
#include <SystemPrv.h>
#include <UI/UIAll.h>
#include <FeatureMgr.h>

static void GccRelocateData ();
static void do_bhook(Word,Ptr,Word);
static void do_ehook(Word,Ptr,Word);
static int StartDebug(void);

register ULong reg_a4 asm("%a4");

ULong
start ()
{
  SysAppInfoPtr appInfo;
  Ptr prevGlobals;
  Ptr globalsPtr;
  ULong save_a4, result;

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

      if (mainFlags & sysAppLaunchFlagNewGlobals)
	{
	  asm volatile ("move.l %a5,%a4; sub.l #edata,%a4");
	  GccRelocateData (); 
	} else {
	  reg_a4 = 0;
	}

      do_bhook(mainCmd, mainPBP, mainFlags);
      StartDebug();
      result = PilotMain (mainCmd, mainPBP, mainFlags);
      do_ehook(mainCmd, mainPBP, mainFlags);
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

/*
 *  This function should be called from 
 */
static void
GccRelocateData ()
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

static void
do_bhook(Word cmd, Ptr PBP, Word flags)
{
    void **hookend, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea bhook_start(%%pc),%0" : "=a" (hookptr) :);
    asm ("lea bhook_end(%%pc),%0" : "=a" (hookend) :);

    while (hookptr < hookend) {
	void (*fptr)(Word,Ptr,Word) = (*(hookptr++)) + text;
	fptr(cmd,PBP,flags);
    }
}

static void
do_ehook(Word cmd, Ptr PBP, Word flags)
{
    void **hookstart, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea ehook_start(%%pc),%0" : "=a" (hookstart) :);
    asm ("lea ehook_end(%%pc),%0" : "=a" (hookptr) :);

    while (hookptr > hookstart) {
	void (*fptr)(Word,Ptr,Word) = (*(--hookptr)) + text;
	fptr(cmd,PBP,flags);
    }
}

static int
StartDebug(void)
{
  DWord feature = 0;

  if (!reg_a4) return 0;

  FtrGet('gdbS', 0, &feature);
  if (feature != 0x12BEEF34) return 0;

#if 0
  /* get the ROM version.  PalmOS2 needs a flag set */
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &feature);
  if (feature >= 0x02000000) {
    (*(char*)257) = 1; /* Turn on debugging */
  }
#endif
  
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
