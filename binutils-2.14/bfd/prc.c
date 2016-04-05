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
#include "safe-ctype.h"
#include "libbfd.h"
#include "prc.h"

static bfd_boolean swap_in_header
  PARAMS ((bfd *abfd, asection *sec, struct palmos_prc_header *header));
static bfd_boolean swap_in_resource_headers
  PARAMS ((bfd *abfd, unsigned int n,
	   struct palmos_prc_resource_header *headers));

static void add_section
  PARAMS ((bfd *abfd, struct palmos_prc_header *header, const char *secname,
           flagword basic_flags, unsigned long offset,
	   unsigned long lim_offset));
static file_ptr tie_down_section
  PARAMS ((bfd *abfd, asection *sec, file_ptr offset));
static void compute_section_file_positions
  PARAMS ((bfd *abfd, file_ptr offset));

/* The PRC file format is documented in the "Palm File Format Specification",
   available from <URL:http://www.palmos.com/dev/support/docs/>.  */

#define HEADER_SIZE	0x4e
#define RSRCENTRY_SIZE	10

/* Read from SEC if non-NULL, otherwise directly from the start of ABFD.  */
static bfd_boolean
swap_in_header (abfd, sec, header)
     bfd *abfd;
     asection *sec;
     struct palmos_prc_header *header;
{
  char raw[HEADER_SIZE];

  if (sec)
    {
      if (! bfd_get_section_contents (abfd, sec, raw, 0, sizeof raw))
	return FALSE;
    }
  else
    {
      if (bfd_seek (abfd, (file_ptr) 0, SEEK_SET) != 0
	  || bfd_bread (raw, sizeof raw, abfd) != sizeof raw)
	return FALSE;
    }

  memcpy (header->name, &raw[0], 32);
  header->name[32] = '\0';

  header->flags		  = bfd_h_get_16 (abfd, &raw[0x20]);
  header->version	  = bfd_h_get_16 (abfd, &raw[0x22]);
  header->create_time	  = bfd_h_get_32 (abfd, &raw[0x24]);
  header->mod_time	  = bfd_h_get_32 (abfd, &raw[0x28]);
  header->backup_time	  = bfd_h_get_32 (abfd, &raw[0x2c]);

  header->appinfo_offset  = bfd_h_get_32 (abfd, &raw[0x34]);
  header->sortinfo_offset = bfd_h_get_32 (abfd, &raw[0x38]);

  memcpy (header->type, &raw[0x3c], 4);
  header->type[4] = '\0';

  memcpy (header->creator, &raw[0x40], 4);
  header->creator[4] = '\0';

  header->nextlist_offset = bfd_h_get_32 (abfd, &raw[0x48]);
  header->nresources	  = bfd_h_get_16 (abfd, &raw[0x4c]);

  return TRUE;
}

static bfd_boolean
swap_in_resource_headers (abfd, n, headers)
     bfd *abfd;
     unsigned int n;
     struct palmos_prc_resource_header *headers;
{
  unsigned int i;

  if (bfd_seek (abfd, (file_ptr) HEADER_SIZE, SEEK_SET) != 0)
    return FALSE;

  for (i = 0; i < n; i++)
    {
      char raw[RSRCENTRY_SIZE];
      if (bfd_bread (raw, sizeof raw, abfd) != sizeof raw)
	return FALSE;

      memcpy (headers[i].type, &raw[0], 4);
      headers[i].type[4] = '\0';
      headers[i].id	 = bfd_h_get_16 (abfd, &raw[4]);
      headers[i].offset  = bfd_h_get_32 (abfd, &raw[6]);
    }

  return TRUE;
}

static bfd_boolean
printable_string_p (const char *s)
{
  while (*s)
    if (! ISPRINT (*s++))
      return FALSE;
  return TRUE;
}

/* We use SEC_LINKER_CREATED to signify arcane processing; namely a section,
   such as header information or appinfo, which does not correspond to a
   real resource.  */

static void
add_section (abfd, header, secname, basic_flags, offset, lim_offset)
     bfd *abfd;
     struct palmos_prc_header *header;
     const char *secname;
     flagword basic_flags;
     unsigned long offset, lim_offset;
{
  asection *sec = bfd_make_section (abfd, secname);
  if (sec == NULL)
    return;

  sec->vma = sec->lma = 0;
  sec->filepos = offset;
  bfd_set_section_size (abfd, sec, lim_offset - offset);
  /* FIXME?? sec->_raw_size = lim_offset - offset; */

  sec->flags = (SEC_HAS_CONTENTS | basic_flags
		| (*(backend_data (abfd)->secflags)) (header, secname));

  if (header->flags & 0x0002)
    sec->flags |= SEC_READONLY;
}

bfd_boolean
_bfd_prc_mkobject (abfd)
     bfd *abfd ATTRIBUTE_UNUSED;
{
  return TRUE;
}

const bfd_target *
_bfd_prc_object_p (abfd)
     bfd *abfd;
{
  struct palmos_prc_header h;
  struct palmos_prc_resource_header *resource = NULL;
  unsigned int i;
  struct stat statbuf;

  if (! swap_in_header (abfd, NULL, &h))
    goto invalid_prc;

  resource = bfd_malloc ((h.nresources + 1)
			 * sizeof (struct palmos_prc_resource_header));
  if (resource == NULL)
    goto failed;

  if (! swap_in_resource_headers (abfd, h.nresources, resource))
    goto invalid_prc;

  if (bfd_stat (abfd, &statbuf) < 0)
    goto failed;

  resource[h.nresources].offset = statbuf.st_size;

  /* The PRC format has no useful magic numbers, so checking that a file
     is a valid .prc is a black art.  We check that:
      - name, type, creator, and resource types are all printable ASCII;
      - flags includes 0x1;
      - next_record_list is 0;
      - app_info, sort_info (when they are non-zero), section pointers are
        strictly ascending.  */

  if (! (printable_string_p (h.name)
	 && (h.flags & 0x0001)
	 && printable_string_p (h.type)
	 && printable_string_p (h.creator)
	 && h.nextlist_offset == 0
	 && (h.appinfo_offset == 0 || h.sortinfo_offset == 0
	     || h.appinfo_offset < h.sortinfo_offset)
	 && (h.sortinfo_offset == 0 || h.sortinfo_offset < resource[0].offset)))
    goto invalid_prc;

  for (i = 0; i < h.nresources; i++)
    if (! (printable_string_p (resource[i].type)
	   && resource[i].offset < resource[i + 1].offset))
      goto invalid_prc;

  add_section (abfd, &h, ".header", SEC_LINKER_CREATED, 0, HEADER_SIZE);

  if (h.appinfo_offset)
    add_section (abfd, &h, ".appinfo",
		 SEC_LINKER_CREATED | SEC_ALLOC | SEC_LOAD, h.appinfo_offset,
		 h.sortinfo_offset? h.sortinfo_offset : resource[0].offset);

  if (h.sortinfo_offset)
    add_section (abfd, &h, ".sortinfo",
		 SEC_LINKER_CREATED | SEC_ALLOC | SEC_LOAD, h.sortinfo_offset,
		 resource[0].offset);

  for (i = 0; i < h.nresources; i++)
    {
      char secnamebuf[32];
      char *secname;

      sprintf (secnamebuf, "%s.%u", resource[i].type, resource[i].id);

      /* FIXME: If this ever fails, it'll leak the previous names.  */
      secname = bfd_alloc (abfd, strlen (secnamebuf) + 1);
      if (secname == NULL)
	goto failed;

      strcpy (secname, secnamebuf);
      add_section (abfd, &h, secname, SEC_ALLOC | SEC_LOAD,
		   resource[i].offset, resource[i + 1].offset);
    }

  if (strcmp (h.type, "appl") == 0)
    abfd->flags |= EXEC_P;
  else if (strcmp (h.type, "libr") == 0 || strcmp (h.type, "GLib") == 0)
    abfd->flags |= DYNAMIC;

  bfd_default_set_arch_mach (abfd,
			     backend_data (abfd)->arch,
			     backend_data (abfd)->mach);

  /* FIXME */
  abfd->symcount = 0;

  return abfd->xvec;

  /* Otherwise ABFD was not recognized as being in PRC format.  Jumps to
     <invalid_prc> are asking for a wrong_format error; jumps to <failed>
     imply that something major has just failed and has already set a
     suitable error code.  */

invalid_prc:
  bfd_set_error (bfd_error_wrong_format);

failed:
  if (resource)
    free (resource);

  return NULL;
}

static file_ptr
tie_down_section (abfd, sec, offset)
     bfd *abfd ATTRIBUTE_UNUSED;
     asection *sec;
     file_ptr offset;
{
  sec->filepos = offset;
  return offset + bfd_section_size (abfd, sec);
}

static void
compute_section_file_positions (abfd, offset)
     bfd *abfd;
     file_ptr offset;
{
  asection *sec;

  sec = bfd_get_section_by_name (abfd, ".appinfo");
  if (sec)
    offset = tie_down_section (abfd, sec, offset);

  sec = bfd_get_section_by_name (abfd, ".sortinfo");
  if (sec)
    offset = tie_down_section (abfd, sec, offset);

  for (sec = abfd->sections; sec != NULL; sec = sec->next)
    if (! (sec->flags & SEC_LINKER_CREATED))
      offset = tie_down_section (abfd, sec, offset);
}

bfd_boolean
_bfd_prc_write_object_contents (abfd)
     bfd *abfd;
{
  /* FIXME This is not yet implemented.  */
  compute_section_file_positions (abfd, HEADER_SIZE);
  return TRUE;
}

asymbol *
_bfd_prc_make_empty_symbol (abfd)
     bfd *abfd;
{
  asymbol *sym = (asymbol *) bfd_zalloc (abfd, sizeof (asymbol));
  if (sym)
    sym->the_bfd = abfd;
  return sym;
}

long
_bfd_prc_get_symtab_upper_bound (abfd)
     bfd *abfd;
{
  return (bfd_get_symcount (abfd) + 1) * sizeof (asymbol *);
}

long
_bfd_prc_get_symtab (abfd, alocation)
     bfd *abfd ATTRIBUTE_UNUSED;
     asymbol **alocation ATTRIBUTE_UNUSED;
{
  return 0;
}

void
_bfd_prc_get_symbol_info (abfd, symbol, ret)
     bfd *abfd ATTRIBUTE_UNUSED;
     asymbol *symbol;
     symbol_info *ret;
{
  bfd_symbol_info (symbol, ret);
}

struct flag_meaning
{
  unsigned int mask;
  const char *name;
};

/* xgettext: These names should not be translated, because they correspond
   to constants defined in Palm OS SDKs.  */

static const struct flag_meaning meanings[] =
{
  { 0x0001, "RESOURCE" },
  { 0x0002, "READONLY" },
  { 0x0004, "APPINFO-DIRTY" },
  { 0x0008, "BACKUP" },
  { 0x0010, "OK-TO-INSTALL-NEWER" },
  { 0x0020, "RESET-AFTER-INSTALL" },
  { 0x0040, "COPY-PREVENTION" },
  { 0x0080, "STREAM" },
  { 0x0100, "HIDDEN" },
  { 0x0200, "LAUNCHABLE-DATA" },
  { 0x0400, "RECYCLABLE" },
  { 0x0800, "BUNDLE" },
  { 0x8000, "OPEN" },
  { 0, NULL }
};

/* FIXME: internationalise me! */

bfd_boolean
_bfd_prc_bfd_print_private_bfd_data (abfd, ptr)
     bfd *abfd;
     PTR ptr;
{
  FILE *f = (FILE *) ptr;
  struct palmos_prc_header header;
  unsigned int flags;
  const struct flag_meaning *meaning;

  swap_in_header (abfd, bfd_get_section_by_name (abfd, ".header"), &header);
  flags = header.flags;

  fprintf (f, "\nDatabase Header:\n");

  fprintf (f, "  Name:    %s\n", header.name);
  fprintf (f, "  Flags:  ");

  for (meaning = meanings; meaning->name; meaning++)
    if (flags & meaning->mask)
      {
	fprintf (f, " %s", meaning->name);
	flags &= ~meaning->mask;
      }

  if (flags != 0)
    fprintf (f, " 0x%x", flags);

  fprintf (f, "\n");

  fprintf (f, "  Type:    %s\n", header.type);
  fprintf (f, "  Creator: %s\n", header.creator);
  fprintf (f, "  Version: %u\n", header.version);

  /* FIXME: Output the dates too, if we can be bothered converting from
     Palm OS's seconds-since-1904-01-01T00:00:00 format.  */

  return TRUE;
}
