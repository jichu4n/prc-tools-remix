/* C support library memory functions for Palm OS.

   Author: John Marshall

   This code is in the public domain.  */

#include "MotSyntax.asm"

#ifdef Lmemcpy

	.globl memcpy
	.globl memcpy_reg
memcpy:
	movem.l 4(sp),d0/a0/a1
	exg d0,a1
memcpy_reg:

	/* @@@@ Write me! */
	rts

#endif
#ifdef Lmemcmp

	.globl memcmp
	.globl memcmp_reg
memcmp:
	movem.l 4(sp),d0/a0/a1
	exg d0,a1
memcmp_reg:
	subq.l #1,d0
	jbcs zero

loop:	cmpm.b (a0)+,(a1)+
	dbne d0,loop

	jblo minus
	jbhi plus
	clr.w d0
	subq.l #1,d0
	jbcc loop

zero:	moveq #0,d0
	rts
plus:	moveq #1,d0
	rts
minus:	moveq #-1,d0
	rts

#endif
#ifdef Lbcopy

	.globl bcopy
bcopy:
	move.l 4(sp),a1
	move.l 8(sp),a0
	move.w 12(sp),d0
	moveq #0,d2

	/* Is the source misaligned?  */
	move.l a1,d1
	btst.l d2,d1
	jbeq srcok

	/* Copy a single byte to align the source.  */
	subq.w #1,d0
	jbcs done
	move.b (a1)+,(a0)+

srcok:	/* Is the destination now misaligned?  */
	move.l a0,d1
	btst.l d2,d1
	jbne copy_b	/* If so, we're stuffed: just copy bytes (*).  */

	/* Now we're fully aligned.  Separate out the number of full
	   longwords to be copied.  */
	move.w d0,d1
	and.w #3,d0
	lsr.w #2,d1

	subq.w #1,d1
	jbcs copy_b

loop_l:	move.l (a1)+,(a0)+
	dbra d1,loop_l
	jbra copy_b

loop_b:	move.b (a1)+,(a0)+
copy_b:	dbra d0,loop_b

done:	rts

/* (*) Actually, it is (just) possible to read and write longwords and
   juggle the bits in registers.  But it's hard to write, you don't win
   *that* much time, and you certainly shouldn't do it if you care about
   code space.  Maybe if this makes it to ROM one day...  :-)  */

#endif
#ifdef Lbcmp

	.globl bcmp
bcmp:
	move.l 4(sp),a1
	move.l 8(sp),a0
	move.w 12(sp),d0
	ext.l d0
	bra.w memcmp_reg

#endif
#ifdef Lbzero

	.globl bzero
bzero:
	move.l 4(sp),a0
	move.w 8(sp),d0
	moveq #0,d2

	/* Is the memory block aligned?  */
	move.l a0,d1
	btst.l d2,d1
	jbeq ok

	/* Zero an initial byte to set up for aligned access.  */
	subq.w #1,d0
	jbcs done
	move.b d2,(a0)+

ok:	/* Find the number of full longwords to be zeroed.  */
	move.w d0,d1
	and.w #3,d0
	lsr.w #2,d1

	subq.w #1,d1
	jbcs copy_b

loop_l:	move.l d2,(a0)+
	dbra d1,loop_l
	jbra copy_b

loop_b:	move.b d2,(a0)+
copy_b:	dbra d0,loop_b

done:	rts

#endif
