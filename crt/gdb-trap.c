	#pragma pack(2)

	#define NON_PORTABLE

	#include <Common.h>
	#include <System/Globals.h>
	#include <UI/ScrDriver.h>
	#include <System/SysAll.h>
	#include <UI/UIAll.h>
	#include <FeatureMgr.h>

	void StartDebug(void)
	{
	 extern long data_start, bss_start;
	 extern void start ();
	 DWord romVersion;
	 FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
	 if (romVersion >= 0x02000000) {
	    (*(char*)257) = 1; /* Turn on debugging */
	 }
 
	 asm("
	        lea data_start(%%a4),%%a1
	        move.l %%a1,%%d2
	        lea bss_start(%%a4),%%a1
	        move.l %%a1,%%d1
	        lea start(%%pc),%%a0
	        move.l %%a0,%%d0
	        sub.l #start, %%d0
	        move.l #0x12BEEF34, %%d3
	        trap #8
	        move.l #0, %%d3
        	move.l #0, %%d0
	        move.l #0, %%d1
	        move.l #0, %%d2
	 " : : : "d0", "d1", "d2", "d3", "a1", "a0");
 
	}
