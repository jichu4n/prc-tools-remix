#include <Pilot.h>
#include "palmos_GLib.h"

#define USE_DYNAMIC_MEM_ONLY

typedef struct {
    void *sp;
    void *a4;
    void *retpc;
    void *pad;
} SaveEntry;

void jmptable();
static void GccRelocateData(void);
static void AllocSaveTable(void);
static void clean(UInt,struct LibRef *);
void crt0_trampoline(void);
static void do_bhook(Word,Ptr,Word);
static void do_ehook(Word,Ptr,Word);
static void do_ctors(void);
static void do_dtors(void);

register void *reg_a4 asm("%a4");

/* This must be the first piece of data */
static void(*trampoline)(void) = crt0_trampoline;

static SaveEntry *savetable;
static SaveEntry *savetableend;
static VoidHand savetableHand;

/* This must be the first function; there con be no constant strings in
   this function, or else they'd end up before "start" in the text segment */
ULong start(UInt ref, struct LibRef *libref,
    void (**cleanfcnP)(UInt, struct LibRef *))
{
    void *save_a4 = reg_a4;
    VoidHand dataH;
    unsigned char *dataP;
    unsigned char *readp;
    unsigned char *writep;
    int i;
    ULong datasize;

    libref->jmptable = (void *)&jmptable;

    /* Load the globals */
    asm("movel #edata,%0" : "=r" (datasize) :);
#ifdef USE_DYNAMIC_MEM_ONLY
    libref->globals = MemPtrNew(datasize);
    /* Don't forget to initialize the bss to 0 ! */
    MemSet(libref->globals, datasize, 0);
    MemPtrSetOwner(libref->globals, 0);
#else
    dataH = DmNewHandle(libref->dbref, datasize);
    MemHandleSetOwner(dataH, 0);
    libref->globals = MemHandleLock(dataH);
    /* No constant strings, remember? */
    /* ErrFatalDisplayIf(!libref->globals, "Cannot allocate globals"); */
#endif
    if (!ref && cleanfcnP) *cleanfcnP = clean;
    dataH = DmGet1Resource('data', 0);
    if (!dataH) return 0;
    dataP = MemHandleLock(dataH);
    if (!dataP) {
	DmReleaseResource(dataH);
	return 0;
    }

    /* Decompress the data segment */
    readp = dataP + 4;
    for(i=0;i<3;++i) {
	ULong startat = *(readp++);
	startat <<= 8; startat += *(readp++);
	startat <<= 8; startat += *(readp++);
	startat <<= 8; startat += *(readp++);
	writep = ((unsigned char *)(libref->globals)) + datasize +
		    (Long)startat;
	while(*readp) {
	    if (*readp & 0x80) {
		unsigned char j;
		unsigned char len = (*(readp++) & 0x7f);
		++len;
		for(j=0;j<len;++j) {
		    *(writep++) = *(readp++);
		}
	    } else if (*readp & 0x40) {
		unsigned char len = (*(readp++) & 0x3f);
		++len;
		writep += len;
	    } else if (*readp & 0x20) {
		unsigned char j;
		unsigned char len = (*(readp++) & 0x1f);
		unsigned char b = *(readp++);
		len += 2;
		for(j=0;j<len;++j) {
		    *(writep++) = b;
		}
	    } else if (*readp & 0x10) {
		unsigned char j;
		unsigned char len = (*(readp++) & 0x0f);
		++len;
		for(j=0;j<len;++j) {
		    *(writep++) = 0xff;
		}
	    } else if (*readp == 1) {
		writep += 4;
		*(writep++) = 0xff;
		*(writep++) = 0xff;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
	    } else if (*readp == 2) {
		writep += 4;
		*(writep++) = 0xff;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
	    } else if (*readp == 3) {
		*(writep++) = 0xa9;
		*(writep++) = 0xf0;
		writep += 2;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		writep += 1;
		*(writep++) = *(readp++);
	    } else if (*readp == 4) {
		*(writep++) = 0xa9;
		*(writep++) = 0xf0;
		writep += 1;
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		*(writep++) = *(readp++);
		writep += 1;
		*(writep++) = *(readp++);
	    }
	}
	++readp;
    }
    MemHandleUnlock(dataH);
    DmReleaseResource(dataH);
    reg_a4 = libref->globals;
    GccRelocateData();
    do_ctors();
    AllocSaveTable();
    do_bhook(0, NULL, 0);
    reg_a4 = save_a4;
    return 0;
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

/* Free the globals we allocated in start() */
static void clean(UInt dummy, struct LibRef *libref)
{
    void *save_a4 = reg_a4;
#ifndef USE_DYNAMIC_MEM_ONLY
    VoidHand orighand;
#endif

    reg_a4 = libref->globals;
    MemHandleUnlock(savetableHand);
    MemHandleFree(savetableHand);
    do_ehook(0, NULL, 0);
    do_dtors();
    reg_a4 = save_a4;
#ifdef USE_DYNAMIC_MEM_ONLY
    MemPtrFree(libref->globals);
#else
    orighand = MemPtrRecoverHandle(libref->globals);
    ErrFatalDisplayIf(!orighand, "Cannot recover handle");
    MemHandleUnlock(orighand);
    MemHandleFree(orighand);
#endif

    asm("
    bras cleanout
    |
    | When this function is called, the stack contains:
    |    fcnid, LibRef, retpc, arg1, arg2, ...
    |
crt0_trampoline:
    |
    | First save the current %%a4 and the retpc in a table keyed by &retpc
    |
    movea.l 4(%%sp),%%a0
    pea 8(%%sp)
    move.l %%a4,-(%%sp)
    movea.l 4(%%a0),%%a4
    |
    | We now have the lib's globals
    |
    bsr.w crt0SaveEntry
    lea 8(%%sp),%%sp
    move.l (%%sp)+,%%d0
    move.l (%%sp)+,%%a0
    lea 4(%%sp),%%sp
    |
    | The stack now starts with the args.  %%d0 contains the fcnid,
    | %%a0 contains the LibRef
    |
    movea.l (%%a0),%%a0
    add.w %%d0,%%d0
    add.w (%%a0,%%d0.w),%%a0
    jsr %%a0@
    |
    | We need to save the function's return value, restore the app's globals
    | and put the old retpc back on the stack
    |
    lea -4(%%sp),%%sp
    move.l %%a0,-(%%sp)
    move.l %%d0,-(%%sp)
    pea 8(%%sp)
    bsr.w crt0GetEntry
    lea 4(%%sp),%%sp
    movea.l %%a0,%%a4
    move.l (%%sp)+,%%d0
    move.l (%%sp)+,%%a0
    |
    | We should now have the app's globals, and the old retpc should be on
    | the top of the stack
    |
    rts

cleanout:
    " : : );
}

static void AllocSaveTable(void)
{
    /* Set up the SaveTable */
    int i;

    savetableHand = MemHandleNew(sizeof(SaveEntry)*4);
    ErrFatalDisplayIf(!savetableHand, "Could not allocate SaveTable");
    MemHandleSetOwner(savetableHand, 0);
    savetable = MemHandleLock(savetableHand);
    ErrFatalDisplayIf(!savetable, "Could not lock SaveTable");
    for (i=0;i<4;++i) savetable[i].sp = 0;
    savetableend = savetable + 4;
}

/* By the time this is called, we have our lib globals */
void crt0SaveEntry(void *app_a4, void **retpcP)
{
    SaveEntry *p = savetable;
    SaveEntry *pe = savetableend;
    while(p != pe) {
	if (!p->sp) {
	    /* Here's a free spot */
	    p->sp = retpcP;
	    p->a4 = app_a4;
	    p->retpc = *retpcP;
	    return;
	}
	++p;
    }
    /* Not enough room in the table.  We could grow the table, but for
       now, we'll just error out. */
    ErrFatalDisplayIf(1, "SaveTable is full");
}

/* Get a saved tuple from the SaveTable.  This routine returns the app_a4. */
void *crt0GetEntry(void **retpcP)
{
    SaveEntry *p = savetable;
    SaveEntry *pe = savetableend;
    while(p != pe) {
	if (p->sp == retpcP) {
	    /* Found it. */
	    p->sp = 0;
	    *retpcP = p->retpc;
	    return p->a4;
	}
	++p;
    }
    /* Not found. */
    ErrFatalDisplayIf(1, "Entry not found in SaveTable");
    return NULL;
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
