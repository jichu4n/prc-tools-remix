/* Madly hand-optimized versions of

   strlen
   strcpy strcat strcmp
   strncpy strncat strncmp
   memcpy memcmp memset

   Also weird _reg forms of each of these for assembly language
   programmers.  Yes, I got very carried away late one night.  :-)  */

/* Since we're using the preprocessor anyway, avoid those percent signs,
   so we can write in something closer to Motorola syntax.  */

#define d0  %d0
#define d1  %d1
#define d2  %d2
#define d3  %d3
#define d4  %d4
#define d5  %d5
#define d6  %d6
#define d7  %d7
#define a0  %a0
#define a1  %a1
#define a2  %a2
#define a3  %a3
#define a4  %a4
#define a5  %a5
#define a6  %a6
#define fp  %fp
#define sp  %sp

#ifdef L_all
#define L_strlen
#define L_strcpy
#define L_strcat
#define L_strcmp
#define L_strncpy
#define L_strncat
#define L_strncmp
#endif

#ifdef L_strlen
	.globl strlen
	.globl strlen_reg
strlen:
	movea.l 4(sp),a0
strlen_reg:
	move.l a0,d0

1:	tst.b (a0)+
	bne.s 1b

	exg d0,a0
	subq.l #1,d0
	sub.l a0,d0
	rts
#endif

#ifdef L_strcpy
	.globl strcpy
	.globl strcpy_reg
strcpy:
	movem.l 4(sp),d0/a0
	movea.l d0,a1
strcpy_reg:
1:	move.b (a0)+,(a1)+
	bne.s 1b
	movea.l d0,a0
	rts
#endif

#ifdef L_strcat
	.globl strcat
	.globl strcat_reg
strcat:
	movem.l 4(sp),d0/a0
	movea.l d0,a1
strcat_reg:
1:	tst.b (a1)+
	bne.s 1b
	subq.l #1,a1
2:	move.b (a0)+,(a1)+
	bne.s 2b
	movea.l d0,a0
	rts
#endif

#ifdef L_strcmp
	.globl strcmp
	.globl strcmp_reg
strcmp:
	movem.l 4(sp),a0/a1
strcmp_reg:
	clr.l d0
	clr.l d1
1:	move.b (a0)+,d0
	move.b (a1)+,d1
	beq.s 2f
	sub.l d1,d0
	beq.s 1b
2:	rts
#endif

#ifdef L_strncpy
	.globl strncpy
	.globl strncpy_reg
strncpy:
	movem.l 4(sp),d0/a0/a1
	exg d0,a1
strncpy_reg:
	move.l a1,d1
	bra.s 2f

1:	move.b (a0)+,(a1)+
2:	dbeq d0,1b
	movea.l d1,a0
	clr.b d1
	subq.w #2,d0
	ble.s 4f

3:	move.b d1,(a1)+
	dbra d0,3b
4:	rts
#endif

#ifdef L_memcmp
	.globl memcmp
	.globl memcmp_reg

memcmp:
	movem 4(sp),d0/a0/a1
	exg d0,a1

#if 0
	/* Do the bottom bits of the two addresses differ? */
	move.l a0,d1
	add.l a1,d1
	lsr.b #1,d1
	bcs Lmisaligned
#endif

Lmisaligned:
	bra.s lstart
loop:	cmpm.b (a1)+,(a0)+
lstart:	dbne d0,loop
	bgt.s plus
	blt.s minus
	clr.w d0
	subq.l #1,d0
	bcc.s loop
	moveq #0,d0
	rts
plus:	moveq #1,d0
	rts
minus:	moveq #-1,d0
	rts
#endif
