#ifndef __ENTRYPOINTS_H__
#define __ENTRYPOINTS_H__

#ifdef __GNUC__
#define ENTRYPOINT(EXTRAS)  __attribute__ ((__owngp__, __extralogue__ EXTRAS))
#else
#define ENTRYPOINT(EXTRAS)
#endif

#define APP_ENTRYPOINT ENTRYPOINT(("movea.l %%a5,%%a4; suba.l #edata,%%a4"))


/* Systraps used herein are:
	a021	MemHandleLock
	a035	MemPtrUnlock
	a0b9	SysLibTblEntry
*/

#define FEATURE_ENTRYPOINT  ENTRYPOINT(("bsr%.w __feature_gp"))
#define GLIB_FEATURE_ENTRYPOINT  ENTRYPOINT(("bsr%.w __glib_feature_gp"))

/* SysLibTblEntry returns a SysLibTblEntryPtr; 4(%%a0) refers the the
   globalsP field.  */
#define SYSLIB_POINTER_ENTRYPOINT  ENTRYPOINT(( \
  /* Prologue: */ "\
move.w %0,-(%%sp); trap #15; dc.w 0xa0b9; \
addq.l #2,%%sp; movea.l 4(%%a0),%%a4" \
  ))

#define eptrap__SysTrap(vector)  "trap !!#15; dc.w <" ## #vector ## ">"
#define eptrap__MemPtrUnlock  eptrap__SysTrap(0xa035)

#define SYSLIB_HANDLE_ENTRYPOINT  ENTRYPOINT(( \
  /* Prologue: */ "\
move.w %0,%%d0; bsr.w __syslib_hnd_gp", \
\
  /* Multiple epilogue: */ "\
movem.l %0,-(%%sp); move.l %%a4,-(%%sp); "eptrap__MemPtrUnlock" \
addq.l #4,%%sp; movem.l (%%sp)+,%0", \
\
  /* Single epilogue: / "\
move.l %%a4,-(%%sp); exg %0,%%a4; trap #15; dc.w 0xa035; \
addq.l #4,%%sp; exg %0,%%a4", \ */ "carrot", \
\
  /* Void epilogue: */ "\
move.l %%a4,-(%%sp); trap #15; dc.w 0xa035; addq.l #4,%%sp", \
  /* Far prologue (an inline copy of __syslib_hnd_gp): */ "\
move.w %0,-(%%sp); trap #15; dc.w 0xa0b9; \
move.l 4(%%a0),-(%%sp); trap #15; dc.w 0xa021; \
addq.l #6,%%sp; movea.l %%a0,%%a4" \
  /* No need for far epilogues: the normal ones will do fine */ \
  ))

#define TRAMPOLINE_ENTRYPOINT  ENTRYPOINT(("movea%.l %%a0,%%a4"))

#define GLIB_CALLBACK	CALLBACK (("bsr%.w __glib_find_globals", "", "", \
  "move%.l __text__(%%a4),%%a0; jsr __glib_find_globals(%%a0)"))

#define MANUAL_ENTRYPOINT  ENTRYPOINT(())

#ifdef __GLIB__
#define AUTO_ENTRYPOINT  GLIB_ENTRYPOINT
#else
#define AUTO_ENTRYPOINT  APP_ENTRYPOINT
#endif

#endif
