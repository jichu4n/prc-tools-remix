#include <SysTraps.asm>

#define appInfo_cmd		/*0*/
#define appInfo_cmdPtr		2
#define appInfo_launchFlags	6

#define XX 0

.globl start
start:
	.equ appInfo,-12
	.equ prevGlobals,-8
	.equ globalsPtr,-4
	link %fp,#-12

	pea globalsPtr(%fp)
	pea prevGlobals(%fp)
	pea appInfo(%fp)
	systrap SysAppStartup
	lea 12(%sp),%sp

	tst.w %d0
	jbne .Lexit

	movea.l	appInfo(%fp),%a2

	/* Do the launchFlags include sysAppLaunchFlagNewGlobals? */

	btst.b #2,appInfo_launchFlags+1(%a2)
	jbeq .Lnoinit

	/* Is there an 'rloc' #0 resource? */

	clr.w -(%sp)
	move.l #0x726c6f63,-(%sp)	/* 'rloc' */
	systrap DmGet1Resource
	addq.l #6,%sp
	move.l %a0,%d7
	beq .Ldone_relocs

	/* If so, perform any relocations in the data resource: */

	move.l %d7,-(%sp)
	systrap MemHandleLock

	movea.l %a0,%a2
	move.l %a0,(%sp)
	systrap MemPtrSize

	movea.l	%a5,%a4
	suba.l #edata,%a4	/* 'edata' rears its ugly head! */

	move.l %a4,%a0		/* data */
	jbsr relocate_chain

	lea start(%pc),%a0	/* text */
	jbsr relocate_chain

#ifdef MULTIPLE_CODE
	lea __text__(%a4),%a3
	move.l %d1,(%a3)+

	move.w %d6,-(%sp)
	move.l #0x646f6465,-(%sp)	/* 'code' */
	systrap DmGet1Resource
	addq.l #6,%sp

	move.l %a0,%d1
	jbeq .Lres_missing

	move.l %a0,-(%sp)
	systrap MemHandleLock
	addq.l #4,%sp
	jbsr relocate_chain

.Lres_missing:
	move.l %d1,(%a3)+

#endif

	move.l %d7,(%sp)
	systrap MemHandleUnlock
	move.l %d7,(%sp)
	systrap DmReleaseResource
	addq.l #4,%sp

.Ldone_relocs:
	lea bhook_ctor_start(%pc),%a3
	lea bhook_ctor_end(%pc),%a0
	move.l %a0,%d0

.Lbhooks:
	cmp.l %a3,%d7
	jbls .Ldone_bhooks
	movea.l (%a3)+,%a0
	add.l start(%pc),%a0
	jbsr call_cmd_pbp_flags
	jbra .Lbhooks

.Ldone_bhooks:
#ifdef GDB_STUB
	/* GdbStartDebug() */
#endif

.Lnoinit:
	movea.l PilotMain(%pc),%a0
	jbsr call_cmd_pbp_flags
	move.l %d0,mainResult(%fp)

	lea ehook_dtor_end(%pc),%a3
	/* lea ehook_dtor_start(%pc),%d7 -- d7 already has this value */

.Lehooks:
	cmp.l %a3,%d7
	jbcc .Ldone_bhooks
	movea.l -(%a3),%a0
	add.l start(%pc),%a0
	jbsr call_cmd_pbp_flags
	jbra .Lbhooks

#ifdef MULTIPLE_CODE
	
#endif

	addq.l #4,%sp
	systrap SysAppExit
	move.l (%sp)+,%d0

.Lexit:
	unlk %a6
	rts

call_cmd_pbp_flags:
	move.w appInfo_cmd(%a2),-(%sp)
	move.l appInfo_cmdPtr(%a2),-(%sp)
	move.w appInfo_launchFlags(%a2),-(%sp)
	jmp (%a0)

relocate_chain:
	move.l %a0,%d1
	move.w (%a2)+,%d0
	jbra .Ltest
.Lloop:	lea (%d0.w,%a4),%a0
	move.w (%a0),%d0
	clr.w (%a0)
	add.l %d1,(%a0)
.Ltest:	cmp.w #0xffff,%d0
	bne.s .Lloop
	rts
