#include <System/SysAll.h>
#include <System/FeatureMgr.h>
#include <System/DataMgr.h>
#include "palmos_GLib.h"

/* This function is called when we need to get a pointer to the LibRef for
   the shared library.  We find the pointer in a Feature if it's already
   open.  If not, we open it ourselves, lock it down, and call its start()
   function, which will fill in bits of the required structure (in which
   case we'll make the Feature for it).  Note that we do not necessarily
   have globals available when we are called. */
struct LibRef *GLibOpen(ULong creator, CharPtr libname) {
    struct LibRef *libref;
    Err err;

    err = FtrGet(creator, 0, (DWord *)&libref);
    if (err || !libref) {
	DmOpenRef dbref;
	VoidHand GLib0;
	ULong (*libstart)(UInt, struct LibRef*,
	    void (**)(UInt, struct LibRef*));
	Char errmsg[80];

	/* Try to load the library */
	dbref = DmOpenDatabaseByTypeCreator('GLib', creator, dmModeReadOnly);
	if (!dbref) {
	    StrCopy(errmsg, libname);
	    StrCat(errmsg, " is not installed");
	    ErrFatalDisplayIf(1, errmsg);
	}

	/* The GLib 0 resource is the code for the library; lock it down */
	GLib0 = DmGet1Resource('GLib', 0);
	if (!GLib0) {
	    StrCopy(errmsg, "Could not load GLib 0 resource for ");
	    StrCat(errmsg, libname);
	    ErrFatalDisplayIf(1, errmsg);
	}
	libstart = MemHandleLock(GLib0);
	if (!libstart) {
	    StrCopy(errmsg, "Could not lock GLib 0 resource for ");
	    StrCat(errmsg, libname);
	    ErrFatalDisplayIf(1, errmsg);
	}

	/* Create a LibRef */
	libref = MemPtrNew(sizeof(struct LibRef));
	if (!libref) {
	    StrCopy(errmsg, "Could not allocate LibRef for ");
	    StrCat(errmsg, libname);
	    ErrFatalDisplayIf(1, errmsg);
	}
	MemPtrSetOwner(libref, 0);

	/* Fill in the LibRef */
	libref->GLibHand = GLib0;
	libref->creator = creator;
	libref->refcount = 0;
	libref->dbref = dbref;
	libstart(0, libref, &(libref->cleanfcn));

	/* Set the Feature */
	FtrSet(creator, 0, (DWord)libref);
    }

    /* Note that we now have it open */
    ++(libref->refcount);

    return libref;
}

/* When the app is done with a library, it should call this routine, passing
   the LibRef as an argument.  We decrement the refcount, cleaning up if
   we were the last one.  The caller is expected to forget about the libref
   after calling this function. */
void GLibClose(struct LibRef *libref)
{
    if (!libref || !libref->refcount) return;

    if (--(libref->refcount)) return;

    /* The refcount has dropped to 0 */

    /* Let the library free its globals */
    if (libref->cleanfcn) (libref->cleanfcn)(0, libref);

    MemHandleUnlock(libref->GLibHand);
    DmReleaseResource(libref->GLibHand);
    DmCloseDatabase(libref->dbref);
    FtrUnregister(libref->creator, 0);

    MemPtrFree(libref);
}

void GLib_dispatch_(void)
{
    /* The tricky bit with this dispatch routine is preservation of
       registers.  The callee is supposed to preserve
       a2,a3,a4,a5,d3,d4,d5,d6,d7.  However, we _can't push stuff on our
       stack_; the arguments to the function we're about to call are
       there.  Also, we may not have globals.  Note also that we see the
       opposite problem when we try to call GLibOpen: we need to
       manually preserve a[01] and d[012] across the call. */
    asm ("
.global GLibDispatch
GLibDispatch:
    |
    | At this point, the stack looks like:
    |   retpc, arg1, arg2, ...
    | the creator id is in %%d1, the libname is pointed to by %%d2,
    | the function code is in %%d0, and if %%a4 is not 0, a pointer to
    | libref is in %%a1.
    |
    cmp.l #0,%%a4
    jbeq noglobals
    move.l (%%a1),%%a0
    cmp.l #0,%%a0
    jbne RefInA0
    move.l %%a1,-(%%sp)
    move.l %%d0,-(%%sp)
    move.l %%d2,-(%%sp)
    move.l %%d1,-(%%sp)
    bsr.w GLibOpen
    lea 8(%%sp),%%sp
    move.l (%%sp)+,%%d0
    move.l (%%sp)+,%%a1
    move.l %%a0,(%%a1)
    bras RefInA0

noglobals:
    move.l %%d0,-(%%sp)
    move.l %%d2,-(%%sp)
    move.l %%d1,-(%%sp)
    bsr.w GLibOpen
    lea 8(%%sp),%%sp
    move.l (%%sp)+,%%d0
RefInA0:
    |
    | At this point, %%a0 contains the address of the LibRef and %%d0 contains
    | the function code.  We find the trampoline function pointed to by the
    | first long in the library's globals.
    |
    move.l 4(%%a0),%%a1
    move.l %%a0,-(%%sp)
    move.l %%d0,-(%%sp)
    move.l (%%a1),%%a1
    jmp %%a1@
    " : : );
}
