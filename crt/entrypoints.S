#include <SysTraps.asm>
#include <MotSyntax.asm>

	.globl __syslib_hnd_gp
__syslib_hnd_gp:
	move.w d0,-(sp)
	systrap SysLibTblEntry
	move.l 4(a0),-(sp)
	systrap MemHandleLock
	addq.l #6,sp
	movea.l a0,a4
	rts
