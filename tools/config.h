/* config.h: general header file for build-prc.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.

   This program, build-prc v2.0, follows in the footsteps of obj-res and
   build-prc, the source code of which contains the following notices:

 * obj-res.c:  Dump out .prc compatible binary resource files from an object
 *
 * (c) 1996, 1997 Dionne & Associates
 * jeff@ryeham.ee.ryerson.ca
 *
 * This is Free Software, under the GNU Public Licence v2 or greater.
 *
 * Relocation added March 1997, Kresten Krab Thorup 
 * krab@california.daimi.aau.dk

 * ptst.c:  build a .prc from a pile of files.
 *
 * (c) 1996, Dionne & Associates
 * (c) 1997, The Silver Hammer Group Ltd.
 * This is Free Software, under the GNU Public Licence v2 or greater.
 */

/* This file contains a table of the symbolic names of all the system traps.
   For example, sysTrapMemPtrNew is trap #0xA013, so there is a line:
	<MemPtrNew> 0xA013
   lookup_trapno(name) (in def.y) searches for "<name>", so anything not
   enclosed in angle brackets constitutes a comment.  */

#define TRAPNO_FNAME	"/usr/local/m68k-palmos-coff/lib/systraps"
