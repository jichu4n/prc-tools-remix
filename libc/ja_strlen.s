	.globl strlen
	.globl strlen_reg
strlen:
	movea.l 4(%sp),%a0
strlen_reg:
	move.l %a0,%d0

.L1:	tst.b (%a0)+
	bne.s .L1

	exg %d0,%a0
	subq.l #1,%d0
	sub.l %a0,%d0
	rts
