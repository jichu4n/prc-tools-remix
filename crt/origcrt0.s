	.file	"origcrt0.c"
gcc2_compiled.:
__gnu_compiled_c:
.text
	.even
.globl start
start:
	link.w %a6,#-12
	movm.l #0x1f20,-(%sp)
	clr.w %d7
	move.l %a4,%a2
	pea -4(%a6)
	pea -8(%a6)
	pea -12(%a6)
	trap #15
	dc.w 0xa08f
	lea (12,%sp),%sp
	tst.w %d0
	jbeq .L2
	move.b #3,-(%sp)
	trap #15
	dc.w 0xa234
	move.l %a2,%a4
	moveq.l #-1,%d0
	bra .L8
	.even
.L2:
	move.l -12(%a6),%a0
	move.w (%a0),%d5
	move.l 2(%a0),%d4
	move.w 6(%a0),%d3
	move.w %d3,%d0
	and.w #20,%d0
	jbeq .L4
#APP
	move.l %a5,%a4; sub.l #edata,%a4
#NO_APP
	bra .L5
	.even
.L4:
	sub.l %a4,%a4
.L5:
	btst #2,%d3
	jbeq .L6
	bsr.w GccRelocateData
	bsr.w do_ctors
	moveq.l #1,%d7
.L6:
	move.w %d3,-(%sp)
	move.l %d4,-(%sp)
	move.w %d5,-(%sp)
	bsr.w do_bhook
	move.w %d3,-(%sp)
	move.l %d4,-(%sp)
	move.w %d5,-(%sp)
	bsr.w PilotMain
	move.l %d0,%d6
	move.w %d3,-(%sp)
	move.l %d4,-(%sp)
	move.w %d5,-(%sp)
	bsr.w do_ehook
	lea (24,%sp),%sp
	tst.w %d7
	jbeq .L7
	bsr.w do_dtors
.L7:
	move.l -4(%a6),-(%sp)
	move.l -8(%a6),-(%sp)
	move.l -12(%a6),-(%sp)
	trap #15
	dc.w 0xa090
	move.l %a2,%a4
	move.l %d6,%d0
.L8:
	movm.l -36(%a6),#0x4f8
	unlk %a6
	rts
.data
	.even
done.4:
	.word 0
.text
.LC0:
	.ascii "unknown reloc.type\0"
.LC1:
	.ascii "origcrt0.c\0"
.LC2:
	.ascii "Unknown reloc.section\0"
	.even
GccRelocateData:
	link.w %a6,#-12
	movm.l #0x1f30,-(%sp)
	lea data_start@END.w(%a5),%a1 /*FRED7b*/
	move.l %a1,-4(%a6)
	lea bss_start@END.w(%a5),%a1 /*FRED7b*/
	move.l %a1,-8(%a6)
	lea start(%pc),%a1 /*SPECIAL_FRED*/
	move.l %a1,-12(%a6)
	lea done.4@END.w(%a5),%a0 /*FRED7b*/
	tst.w (%a0)
	jbne .L9
	move.w #1,(%a0)
#APP
	sub.l #start, %a1
#NO_APP
	move.l %a1,-12(%a6)
#APP
	sub.l #bss_start, -8(%a6)
	sub.l #data_start, -4(%a6)
#NO_APP
	clr.w -(%sp)
	move.l #1919709027,-(%sp)
	trap #15
	dc.w 0xa060
	move.l %a0,%d7
	addq.l #6,%sp
	jbeq .L9
	move.l %d7,-(%sp)
	trap #15
	dc.w 0xa021
	move.w (%a0)+,%a3
	move.l %a0,%a2
	clr.w %d5
	addq.l #4,%sp
	cmp.w %a3,%d5
	jbcc .L14
	moveq.l #0,%d6
	clr.w %d4
	.even
.L16:
	move.w %d5,%d6
	move.l %d6,%d3
	lsl.l #3,%d3
	cmp.b #190,(%a2,%d3.l)
	jbeq .L17
	lea .LC0(%pc),%a1 /*SPECIAL_FRED*/
	move.l %a1,-(%sp)
	move.w #158,-(%sp)
	lea .LC1(%pc),%a1 /*SPECIAL_FRED*/
	move.l %a1,-(%sp)
	trap #15
	dc.w 0xa084
	lea (10,%sp),%sp
.L17:
	clr.l %d1
	move.w 2(%a2,%d3.l),%d1
	lea data_start@END.w(%a5),%a1 /*FRED7b*/
	move.l %a1,%d0
	move.l %d1,%a0
	add.l %d0,%a0 /*JANE2*/
	move.b 1(%a2,%d3.l),%d4
	cmp.w #100,%d4
	jbeq .L20
	jbgt .L24
	cmp.w #98,%d4
	jbeq .L21
	bra .L22
	.even
.L24:
	cmp.w #116,%d4
	jbne .L22
	move.l -12(%a6),%d2
	add.l %d2,(%a0) /*JANE2*/
	bra .L15
	.even
.L20:
	move.l -4(%a6),%d2
	add.l %d2,(%a0) /*JANE2*/
	bra .L15
	.even
.L21:
	move.l -8(%a6),%d2
	add.l %d2,(%a0) /*JANE2*/
	bra .L15
	.even
.L22:
	lea .LC2(%pc),%a1 /*SPECIAL_FRED*/
	move.l %a1,-(%sp)
	move.w #177,-(%sp)
	lea .LC1(%pc),%a1 /*SPECIAL_FRED*/
	move.l %a1,-(%sp)
	trap #15
	dc.w 0xa084
	lea (10,%sp),%sp
.L15:
	addq.w #1,%d5
	cmp.w %a3,%d5
	jbcs .L16
.L14:
	move.l %d7,-(%sp)
	trap #15
	dc.w 0xa022
	move.l %d7,-(%sp)
	trap #15
	dc.w 0xa061
.L9:
	movm.l -40(%a6),#0xcf8
	unlk %a6
	rts
	.even
do_bhook:
	link.w %a6,#0
	movm.l #0x1e30,-(%sp)
	move.w 8(%a6),%d6
	move.l 10(%a6),%d5
	move.w 14(%a6),%d4
	lea start(%pc),%a2 /*SPECIAL_FRED*/
#APP
	sub.l #start, %a2
	lea bhook_start,%a3; add.l %a2,%a3
	lea bhook_end,%a1; add.l %a2,%a1
#NO_APP
	move.l %a1,%d3
	cmp.l %a3,%d3
	jbls .L28
	.even
.L29:
	move.l (%a3)+,%a0
	add.l %a2,%a0 /*JANE2*/
	move.w %d4,-(%sp)
	move.l %d5,-(%sp)
	move.w %d6,-(%sp)
	jsr (%a0)
	addq.l #8,%sp
	cmp.l %a3,%d3
	jbhi .L29
.L28:
	movm.l -24(%a6),#0xc78
	unlk %a6
	rts
	.even
do_ehook:
	link.w %a6,#0
	movm.l #0x1e30,-(%sp)
	move.w 8(%a6),%d6
	move.l 10(%a6),%d5
	move.w 14(%a6),%d4
	lea start(%pc),%a3 /*SPECIAL_FRED*/
#APP
	sub.l #start, %a3
	lea ehook_start,%a1; add.l %a3,%a1
#NO_APP
	move.l %a1,%d3
#APP
	lea ehook_end,%a2; add.l %a3,%a2
#NO_APP
	cmp.l %a2,%d3
	jbcc .L33
	.even
.L34:
	move.l -(%a2),%a0
	add.l %a3,%a0 /*JANE2*/
	move.w %d4,-(%sp)
	move.l %d5,-(%sp)
	move.w %d6,-(%sp)
	jsr (%a0)
	addq.l #8,%sp
	cmp.l %a2,%d3
	jbcs .L34
.L33:
	movm.l -24(%a6),#0xc78
	unlk %a6
	rts
	.even
do_ctors:
	link.w %a6,#0
	movm.l #0x1030,-(%sp)
	lea start(%pc),%a3 /*SPECIAL_FRED*/
#APP
	sub.l #start, %a3
	lea ctors_start,%a2; add.l %a3,%a2
	lea ctors_end,%a1; add.l %a3,%a1
#NO_APP
	move.l %a1,%d3
	cmp.l %a2,%d3
	jbls .L38
	.even
.L39:
	move.l (%a2)+,%a0
	add.l %a3,%a0 /*JANE2*/
	jsr (%a0)
	cmp.l %a2,%d3
	jbhi .L39
.L38:
	movm.l -12(%a6),#0xc08
	unlk %a6
	rts
	.even
do_dtors:
	link.w %a6,#0
	movm.l #0x1030,-(%sp)
	lea start(%pc),%a3 /*SPECIAL_FRED*/
#APP
	sub.l #start, %a3
	lea dtors_start,%a1; add.l %a3,%a1
#NO_APP
	move.l %a1,%d3
#APP
	lea dtors_end,%a2; add.l %a3,%a2
#NO_APP
	cmp.l %a2,%d3
	jbcc .L43
	.even
.L44:
	move.l -(%a2),%a0
	add.l %a3,%a0 /*JANE2*/
	jsr (%a0)
	cmp.l %a2,%d3
	jbcs .L44
.L43:
	movm.l -12(%a6),#0xc08
	unlk %a6
	rts
