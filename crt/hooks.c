/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 *
 *  Modified 19971111 by Ian Goldberg <iang@cs.berkeley.edu>
 *  Modified 19981104 by John Marshall  <jmarshall@acm.org>
 */

/* All we really want is Common.h/PalmTypes.h.  This is a header which gives
   us that and not too much else.  */
#include <ErrorMgr.h>

#include "NewTypes.h"
#include "crt.h"

extern UInt32 start ();

void
__do_bhook (UInt16 cmd, void *PBP, UInt16 flags)
{
    void **hookend, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea bhook_start,%0; add.l %1,%0" : "=a" (hookptr) : "g" (text));
    asm ("lea bhook_end,%0; add.l %1,%0" : "=a" (hookend) : "g" (text));

    while (hookptr < hookend) {
	void (*fptr)(UInt16,void*,UInt16) = (*(hookptr++)) + text;
	fptr(cmd,PBP,flags);
    }
}

void
__do_ehook (UInt16 cmd, void *PBP, UInt16 flags)
{
    void **hookstart, **hookptr;
    unsigned long text = (unsigned long)&start;
    asm ("sub.l #start, %0" : "=g" (text) : "0" (text));

    asm ("lea ehook_start,%0; add.l %1,%0" : "=a" (hookstart) : "g" (text));
    asm ("lea ehook_end,%0; add.l %1,%0" : "=a" (hookptr) : "g" (text));

    while (hookptr > hookstart) {
	void (*fptr)(UInt16,void*,UInt16) = (*(--hookptr)) + text;
	fptr(cmd,PBP,flags);
    }
}

void
__do_ctors (void)
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

void
__do_dtors (void)
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
