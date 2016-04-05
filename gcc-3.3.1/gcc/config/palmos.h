/* Definitions for Palm OS.
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

/* Some Palm OS headers in TOOL_INCLUDE_DIR need to #include_next their
   corresponding header in CROSS_INCLUDE_DIR, so we need to reorder them
   from the default.  */
#define INCLUDE_DEFAULTS			\
  {						\
    { GCC_INCLUDE_DIR, "GCC", 0, 0 },		\
    { TOOL_INCLUDE_DIR, "BINUTILS", 0, 1 },	\
    { CROSS_INCLUDE_DIR, 0, 0, 0 },		\
    { 0, 0, 0, 0 }				\
  }

/* Palm OS headers are C++-aware.  */
#define NO_IMPLICIT_EXTERN_C

/* Palm OS programmers enjoy (ab)using all kinds of packing pragmas.  */
#define HANDLE_SYSV_PRAGMA 1
#define HANDLE_PRAGMA_PACK_PUSH_POP 1

/* Add Palm OS-related predefines to any generic ones.  */
#undef CPP_PREDEFINES
#define CPP_PREDEFINES "-D__palmos__ -Asystem=palmos"
