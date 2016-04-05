/* Target machine definitions for m68k Palm OS.
   Copyright 2002 John Marshall.  (For now.)

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* GCC is probably the only compiler used on this configuration.  So
   get this right even if the code which detects gcc2_compiled. is
   still broken.  */

#define BELIEVE_PCC_PROMOTION 1

/* The target system handles breakpoints, so we don't have to correct for
   them.  */

#define DECR_PC_AFTER_BREAK 0

/* No float registers.  */

/*#define NUM_REGS 18*/

#include "m68k/tm-m68k.h"

#define TARGET_INT_BIT 16
#define TARGET_PTR_BIT 32

#undef REGISTER_VIRTUAL_TYPE
#define REGISTER_VIRTUAL_TYPE(N)                                \
 (((unsigned)(N) - FP0_REGNUM) < 8 ? builtin_type_double :      \
 (N) == PC_REGNUM || (N) == FP_REGNUM || (N) == SP_REGNUM ?     \
 lookup_pointer_type (builtin_type_void) : builtin_type_long)

/* FIXME, should do GET_LONGJMP_TARGET for newlib.  */

#ifdef __GNUC__
#define MAC_SYSCALL_TRAP \
   ({ static long addr = 0; \
      addr != 0 ? addr : (addr = read_memory_integer ((32+15)*4, 4)); })
#else
#define MAC_SYSCALL_TRAP read_memory_integer ((32+15)*4, 4)
#endif
