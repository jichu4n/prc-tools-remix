/* C support library string conversion functions for Palm OS.

   Author: John Marshall

   This code is in the public domain.  */

#include "MotSyntax.asm"

/* Anyone feel like writing strtod?!  */

/* Why on earth are these in assembler instead of C?  Because it's heaps
   easier get the overflow conditions right.  And I got a little bored
   while repeatedly recompiling gcc late one night...  */

#ifdef Lstrtol

	.globl strtol
strtol:

	move.b (a0)+,d0
	cmp.b 'a',d0
	jblo L1
	sub.b #('a'-10),d0
	jbra digit
L1:	cmp.b 'A',d0
	jblo L2
	sub.b #('A'-10),d0
	jbra digit
L2:	cmp.b '9',d0
	jbhi invalid
	cmp.b '0',d0
	jblo invalid
	sub.b #'0',d0
digit:	cmp.b d1,d0
	jbhs invalid
	

#endif
#ifdef Lstrtoul

	.globl strtoul
strtoul:


ws:	move.b (a0)+,d0
	jbeq eos
	cmp.b ' ',d0
	jbeq ws		/* @@@@ No!! Use isspace() */

	moveq #0,d2
	cmp.b '+',d0
	jbeq skip
	cmp.b '-',d0
	jbne nosign
	moveq #1,d2
skip:	move.b (a0)+,d0
nosign:
	


#endif
#ifdef Latoil

	.globl atoi
	.globl atol
atoi:
atol:
	move.w #10,-(sp)
	clr.l -(sp)
	move.l 10(sp),-(sp)
	bsr.w strtol
	lea 10(sp),sp
	rts

#endif
