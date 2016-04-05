/* Definitions of software floating point exeptions
   Copyright (C) 1997 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */



extern struct m68881_fpCCR _fpCCR;

void (*__fp_exception_handler_ptr)(struct m68881_fpCCR*) = 0;

void
__fp_exception (struct m68881_fpCCR* ccr)
{
  if (__fp_exception_handler_ptr == 0)
    asm volatile ("move.l %0,sp@-; trap #15" : : "g" (ccr));
  else
    (*__fp_exception_handler_ptr) (ccr);
}

struct m68881_fpCCR*
__fp_exception_handler (void (*h) (struct m68881_fpCCR*))
{
  struct m68881_fpCCR* old = __fp_exception_handler_ptr;
  __fp_exception_handler_ptr = h;
  return old;
}


