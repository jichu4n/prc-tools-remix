	.globl strcpy
	.globl strcpy_reg
strcpy:
	movem.l 4(%sp),%d0/%a0
	movea.l %d0,%a1
strcpy_reg:
.L:	move.b (%a0)+,(%a1)+
	bne.s .L
	movea.l %d0,%a0
	rts
