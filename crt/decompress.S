#include <MotSyntax.asm>

/* A couple of notes about this routine:
   * We assume that memory has already been MemSet to 0, and don't actually
     write explicit zeroes, but just advance the pointer.  This saves a
     little bit of code space.
   * Most of the counters in the code are used for dbra loops, so are one
     less than you would expect!  These are marked with cryptic "+1"
     comments.  */

	.globl _GccDecompressData
_GccDecompressData:
	movea.l 8(sp),a0

	moveq #2,d2	/* +1 */

stream_loop:
	/* Is the packed_data pointer aligned? */
	move.l a0,d0
	lsr.b #1,d0
	jbcs unaligned

	movea.l (a0)+,a1
	jbra L1
unaligned:
	move.b (a0)+,d1
	move.l (a0)+,d0
	subq.l #1,a0
	move.b d1,d0
	ror.l #8,d0
	movea.l d0,a1

L1:	adda.l 4(sp),a1

loop:	moveq #0,d0
	move.b (a0)+,d0
	jbmi literal_block
	jbeq stream_end
	cmpi.b #0x40,d0
	jbhs zero_run
	cmpi.b #0x20,d0
	jbhs constant_run
	cmpi.b #0x10,d0
	jbhs minus1_run
	lsl #2,d0
	jmp (-2,pc,d0)
code1:	moveq #-1,d0
	jbra codes_12
code2:	move.b (a0)+,d0
	jbra codes_12
code3:	moveq #0,d0
	jbra codes_34
code4:	move.b (a0)+,d0
	/* jbra codes_34 */

codes_34:
	move.b #0xa9,(a1)+
	move.b #0xf0,(a1)+
	addq.l #1,a1
	move.b d0,(a1)+
	move.b (a0)+,(a1)+
	move.b (a0)+,(a1)+
	addq.l #1,a1
	move.b (a0)+,(a1)+
	jbra loop

codes_12:
	addq.l #4,a1
	st (a1)+
	move.b d0,(a1)+
	move.b (a0)+,(a1)+
	move.b (a0)+,(a1)+
	jbra loop

literal_block:
	subi.w #0x80,d0		/* +1 */
copy:	move.b (a0)+,(a1)+
	dbra d0,copy
	jbra loop

zero_run:
	subi.w #0x3f,d0
	adda.w d0,a1
	jbra loop

constant_run:
	subi.w #0x1f,d0		/* +1 */
	move.b (a0)+,d1
	jbra run

minus1_run:
	subi.w #0x10,d0		/* +1 */
	moveq #-1,d1
	/* jbra run */

run:	move.b d1,(a1)+
	dbra d0,run
	jbra loop

stream_end:
	dbra d2,stream_loop
	rts
