	.globl strncpy
	.globl strncpy_reg
strncpy:
	movem.l 4(%sp),%d0/%a0/%a1
	exg %d0,%a1
strncpy_reg:
	move.l %a1,%d1
	bra.s .Ltop

.Lcopy:	move.b (%a0)+,(%a1)+
.Ltop:	dbeq %d0,.Lcopy
	movea.l %d1,%a0
	clr.b %d1
	subq.w #2,%d0
	ble.s .Lout

.Lzero:	move.b %d1,(%a1)+
	dbra %d0,.Lzero
.Lout:	rts
