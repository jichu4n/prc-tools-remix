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

struct palmos_prc_header
{
  char name[33];
  unsigned int flags, version;
  unsigned long create_time, mod_time, backup_time;
  char type[5], creator[5];
  unsigned long appinfo_offset, sortinfo_offset, nextlist_offset;
  unsigned int nresources;
};

struct palmos_prc_resource_header
{
  char type[5];
  unsigned int id;
  unsigned long offset;
};

#define backend_data(abfd) \
  ((const struct palmos_prc_backend_data *) (abfd)->xvec->backend_data)

struct palmos_prc_backend_data
{
  enum bfd_architecture arch;
  unsigned long mach;
  flagword (*secflags)
    PARAMS ((const struct palmos_prc_header *, const char *));
};

extern bfd_boolean _bfd_prc_mkobject PARAMS ((bfd *));
extern const bfd_target *_bfd_prc_object_p PARAMS ((bfd *));
extern bfd_boolean _bfd_prc_write_object_contents PARAMS ((bfd *));

#define _bfd_prc_bfd_copy_private_bfd_data \
  _bfd_generic_bfd_copy_private_bfd_data
#define _bfd_prc_bfd_merge_private_bfd_data \
  _bfd_generic_bfd_merge_private_bfd_data
#define _bfd_prc_bfd_copy_private_section_data \
  _bfd_generic_bfd_copy_private_section_data
#define _bfd_prc_bfd_copy_private_symbol_data \
  _bfd_generic_bfd_copy_private_symbol_data
#define _bfd_prc_bfd_set_private_flags	  _bfd_generic_bfd_set_private_flags
extern bfd_boolean _bfd_prc_bfd_print_private_bfd_data PARAMS ((bfd *, PTR));

extern long _bfd_prc_get_symtab_upper_bound PARAMS ((bfd *));
extern long _bfd_prc_get_symtab PARAMS ((bfd *, asymbol **));
extern asymbol *_bfd_prc_make_empty_symbol PARAMS ((bfd *));
#define _bfd_prc_print_symbol		  _bfd_nosymbols_print_symbol
extern void _bfd_prc_get_symbol_info PARAMS ((bfd *, asymbol *, symbol_info *));
#define _bfd_prc_bfd_is_local_label_name  bfd_generic_is_local_label_name
#define _bfd_prc_get_lineno		  _bfd_nosymbols_get_lineno
#define _bfd_prc_find_nearest_line	  _bfd_nosymbols_find_nearest_line
#define _bfd_prc_bfd_make_debug_symbol	  _bfd_nosymbols_bfd_make_debug_symbol
#define _bfd_prc_read_minisymbols	  _bfd_generic_read_minisymbols
#define _bfd_prc_minisymbol_to_symbol	  _bfd_generic_minisymbol_to_symbol

#define PRC_TARGET_VECTOR(name, body_endianness,		\
			  getu64, gets64, put64,		\
			  getu32, gets32, put32,		\
			  getu16, gets16, put16,		\
			  backend_data)				\
  {								\
    (name),							\
    bfd_target_unknown_flavour,					\
    (body_endianness),						\
    BFD_ENDIAN_BIG,    /* All PRC headers are big endian.  */	\
    EXEC_P | DYNAMIC,  /* plausible BFD flags  */		\
    (SEC_HAS_CONTENTS | SEC_LINKER_CREATED | SEC_ALLOC		\
     | SEC_LOAD | SEC_READONLY | SEC_CODE | SEC_DATA		\
     | SEC_DEBUGGING),  /* plausible section flags  */		\
    0,    /* symbol prefix  */					\
    ' ',  /* archive pad character  */				\
    16,   /* archive member name length  */			\
    /* Body swapping.  */					\
    (getu64), (gets64), (put64),				\
    (getu32), (gets32), (put32),				\
    (getu16), (gets16), (put16),				\
    /* Header swapping.  */					\
    bfd_getb64, bfd_getb_signed_64, bfd_putb64,			\
    bfd_getb32, bfd_getb_signed_32, bfd_putb32,			\
    bfd_getb16, bfd_getb_signed_16, bfd_putb16,			\
    /* bfd_check_format */					\
    {								\
      _bfd_dummy_target,					\
      _bfd_prc_object_p,					\
      _bfd_dummy_target,					\
      _bfd_dummy_target						\
    },								\
    /* bfd_set_format */					\
    {								\
      bfd_false,						\
      _bfd_prc_mkobject,					\
      bfd_false,						\
      bfd_false							\
    },								\
    /* bfd_write_contents */					\
    {								\
      bfd_false,						\
      _bfd_prc_write_object_contents,				\
      bfd_false,						\
      bfd_false							\
    },								\
    BFD_JUMP_TABLE_GENERIC (_bfd_generic),			\
    BFD_JUMP_TABLE_COPY (_bfd_prc),				\
    BFD_JUMP_TABLE_CORE (_bfd_nocore),				\
    BFD_JUMP_TABLE_ARCHIVE (_bfd_noarchive),			\
    BFD_JUMP_TABLE_SYMBOLS (_bfd_prc),				\
    BFD_JUMP_TABLE_RELOCS (_bfd_norelocs),			\
    BFD_JUMP_TABLE_WRITE (_bfd_generic),			\
    BFD_JUMP_TABLE_LINK (_bfd_nolink),				\
    BFD_JUMP_TABLE_DYNAMIC (_bfd_nodynamic),			\
    NULL,  /* No alternative endianness backend. */		\
    (PTR) (backend_data)					\
  }
