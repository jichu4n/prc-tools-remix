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
      const struct palmos_prc_header *header ATTRIBUTE_UNUSED;
      const char *secname;
{
  flagword flags = 0;

  if (strncmp (secname, "armc", 4) == 0)
    flags |= SEC_CODE;

  return flags;
}

static const struct palmos_prc_backend_data arm_backend_data =
{
  bfd_arch_arm, bfd_mach_arm_4T, secflags
};

const bfd_target prc_littlearm_vec =
  PRC_TARGET_VECTOR ("prc-littlearm", BFD_ENDIAN_LITTLE, 
		     bfd_getl64, bfd_getl_signed_64, bfd_putl64,
		     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
		     bfd_getl16, bfd_getl_signed_16, bfd_putl16,
		     &arm_backend_data);
