/* Definitions for Palm OS on ARM.
   Copyright 2002 John Marshall.  (Until it is contributed.)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#define TARGET_VERSION  fputs (" (ARM/ELF Palm OS armlet)", stderr);

/* Add Palm OS-related predefines to those for generic ARM and ARM/ELF.  */
#define CPP_PREDEFINES "\
-Darm -Darm_elf -D__palmos__ \
-Acpu(arm) -Asystem(palmos) -Amachine(pilot) -D__ELF__"

/* Palm OS headers are C++-aware.  */
#define NO_IMPLICIT_EXTERN_C

#define SUBTARGET_CPU_DEFAULT  TARGET_CPU_arm7tdmi

#include "arm/unknown-elf.h"

/* Palm OS code must be PIC code, and the OS expects %r9 to be fixed.
   We could do the latter with SUBTARGET_CONDITIONAL_REGISTER_USAGE instead,
   but this is more visible for our paranoid users.  */
#undef CC1_SPEC
#define CC1_SPEC  "-fPIC -ffixed-r9"
