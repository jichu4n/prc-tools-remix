	.globl strcmp
	.globl strcmp_reg
strcmp:
	movem.l 4(%sp),%a0/%a1
strcmp_reg:
	clr.l %d0
	clr.l %d1
.L:	move.b (%a0)+,%d0
	move.b (%a1)+,%d1
	beq.s .Lout
	sub.l %d1,%d0
	beq.s .L
.Lout:	rts
