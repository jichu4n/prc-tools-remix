/*
 *  Pilot startup code for use with gcc.  This code was written 
 *  by Kresten Krab Thorup, and is in the public domain.
 *  It is *not* under the GPL or the GLPL, you can freely link it
 *  into your programs.
 *
 *  Modified 19971111 by Ian Goldberg <iang@cs.berkeley.edu>
 *  Modified 19981104 by John Marshall  <jmarshall@acm.org>
 */

#include <Common.h>
#include "crt.h"

void __do_bhook(Word cmd, Ptr PBP, Word flags)
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

void __do_ehook(Word cmd, Ptr PBP, Word flags)
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

void __do_ctors(void)
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

void __do_dtors(void)
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
