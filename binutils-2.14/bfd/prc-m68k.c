/* BFD back-end for the Palm OS PRC resource database format.
   Copyright 2002 John Marshall.  (For now.)
   Contributed by Falch.net as.

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

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "prc.h"

static flagword secflags
  PARAMS ((const struct palmos_prc_header *header, const char *secname));

static flagword
secflags (header, secname)
      const struct palmos_prc_header *header;
      const char *secname;
{
  flagword flags = 0;

  if (strncmp (secname, "libr", 4) == 0
      || strncmp (secname, "GLib", 4) == 0
      || strncmp (secname, "HACK", 4) == 0
      || (strncmp (secname, "code", 4) == 0
	  && (strcmp (secname, "code.0") != 0
	      || strcmp (header->type, "appl") != 0)))
    flags |= SEC_CODE;

  if (strncmp (secname, "data", 4) == 0)
    flags |= SEC_DATA;

  if (strncmp (secname, "gdbS", 4) == 0)
    flags |= SEC_DEBUGGING;

  return flags;
}

/* FIXME Crazy scheme: we could synthesize symbols from MacsBug symbols
   in the code resources.  */

static const struct palmos_prc_backend_data m68k_backend_data =
{
  bfd_arch_m68k, bfd_mach_m68000, secflags
};

const bfd_target prc_m68k_vec =
  PRC_TARGET_VECTOR ("prc-m68k", BFD_ENDIAN_BIG, 
		     bfd_getb64, bfd_getb_signed_64, bfd_putb64,
		     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
		     bfd_getb16, bfd_getb_signed_16, bfd_putb16,
		     &m68k_backend_data);
