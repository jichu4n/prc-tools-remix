/* binres.cpp: extract Palm OS resources from a bfd executable.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.

   This program follows in the footsteps of obj-res and build-prc, the
   source code of which contains the following notices:

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libiberty.h"
#include "bfd.h"

#include "binres.hpp"

#include "pfd.hpp"
#include "utils.h"

#if 0
#include "coff/internal.h"
#include "libcoff.h"
#else
/* It doesn't seem worthwhile to play enough autoconf games to make the
   above include files accessible.  The following is all we really need.  */

/* From coff/internal.h: */
#define R_RELLONG       (0x11)  /* Direct 32-bit relocation */

/* libcoff.h would give us access to howtos and bfd_get_reloc_size, but
   the following should be a good enough approximation.  It's exactly right
   for the only reloc type ever likely to be in a .dreloc section.  */
#define RELOC_SIZE 4
#endif


/* Make a code resource, possibly with one or two extra instructions at
   the start.  */
static pfd_data
make_code (bfd *abfd, asection *sec, database_kind kind, bool maincode) {
  size_t res_size, sec_size;

  res_size = sec_size = bfd_section_size (abfd, sec);

  /* Add space for extra instructions at the beginning.  Jumping to the
     entry point should be useful, but that `or' instruction is only there
     because CodeWarrior puts one there.  It's very mysterious: %d0 is,
     of course, dead at that point.  */

  if (maincode) {
    if (kind == DK_APPLICATION)
      res_size += 4;  /* ori.b #0,%d0 */

    unsigned int entry = bfd_get_start_address (abfd);
    if (entry > 126)  res_size += 4;     /* bra.w entry */
    else if (entry > 0)  res_size += 2;  /* bra.s entry */
    }

  pfd_data res (res_size);
  unsigned char *data = res.writable_contents();

  if (maincode) {
    if (kind == DK_APPLICATION) {
      set_short (data+0, 0x0000);  /* ori.b #IMM,%d0 */
      set_short (data+2, 0x0001);  /* IMM = 1 */
      data += 4;
      }

    unsigned int entry = bfd_get_start_address (abfd);
    if (entry > 32766) {  /* ??? */
      ewhere ("0x%x", entry);
      einfo (E_FILEWHERE, "entry point too distant");
      }
    else if (entry > 126) {
      set_short (data+0, 0x6000);  /* bra.w OFF */
      set_short (data+2, entry);   /* OFF = entry */
      data += 4;
      }
    else if (entry > 0) {
      set_byte (data+0, 0x60);   /* bra.s OFF */
      set_byte (data+1, entry);  /* OFF = entry */
      data += 2;
      }
    }

  if (!bfd_get_section_contents (abfd, sec, data, 0, sec_size))
    einfo (E_FILE, "can't read `%s' section", bfd_section_name (abfd, sec));

  return res;
  }

/* Make the mysterious code #0 resource.  (Needed only by applications.)  */
static pfd_data
make_code0 (size_t total_data_size) {
  pfd_data res (24);
  unsigned char* data = res.writable_contents();

  /* The code #0 resource: as Jeff said "Truth be known, I think it's
     mostly bogus".  Ted Ts'o has a likely-looking theory that it's an
     unadulterated Macintosh code #0 jump table.  I'm not sure why we
     want one of these in Palm OS land :-).  The `(?)'s mark what these
     fields mean if it is indeed a Macintosh resource.

     The early Macintoshes used this jump table to handle multiple code
     resources.  Fortunately, we don't have to support demand loading of
     resources, so our multiple code resource handling is much simpler,
     and doesn't need this jump table.  */

  set_long (data+ 0, 0x00000028);  /* size above %a5 (?) */
  set_long (data+ 4, total_data_size);
  set_long (data+ 8, 8);	   /* size of jump table (?) */
  set_long (data+12, 0x00000020);  /* jump table's offset from %a5 (?) */

  /* The one and only jump table entry: (?) */

  set_short (data+16, 0x0000);  /* offset (?) */
  set_short (data+18, 0x3f3c);  /* move.w #IMM,-(%sp) (?) */
  set_short (data+20, 0x0001);  /* IMM = 1 (?) */
  set_short (data+22, 0xa9f0);  /* Macintosh SegLoad trap (?!) */

  return res;
  }

/* Make a preferences resource.  (Needed only by applications.)  */
static pfd_data
make_pref () {
  pfd_data res (10);
  unsigned char* data = res.writable_contents();

  /* This resource contains a SysAppPrefsType, as described in
     System/SystemPrv.h.  However, probably most of these numbers are
     ignored anyway.  */

  set_short (data+0, 30);    /* AMX task priority */
  set_long  (data+2, 4096);  /* stack size */
  set_long  (data+6, 4096);  /* minimum free space in heap */

  return res;
  }


static int
make_reloc_chains (pfd_data& rloc_res, int nchains,
		   unsigned long resid_from_secndx[], bfd* abfd,
		   unsigned char* datares, size_t datares_size) {
  unsigned char* reloc_heads = rloc_res.writable_contents();
  for (int i = 0; i < nchains; i++)
    set_short (reloc_heads + 2*i, 0xffff);

  asection* relocs_sec = bfd_get_section_by_name (abfd, ".dreloc");
  if (relocs_sec == NULL)  return 0;
  bfd_size_type relocs_size = bfd_section_size (abfd, relocs_sec);
  if (relocs_size == 0)  return 0;

  bfd_byte* relocs = (bfd_byte*) xmalloc (relocs_size);
  if (!bfd_get_section_contents (abfd, relocs_sec, relocs, 0, relocs_size)) {
    einfo (E_FILE, "can't read `.dreloc' section");
    free (relocs);
    return 0;
    }

  int nrelocs = 0;

  for (bfd_byte* rel = relocs; rel < relocs + relocs_size; rel += 8) {
    unsigned int type, reloffset;
    int relsecndx, symsecndx;
    asection *sec, *relsec, *symsec;
    CONST char *relsecname, *symsecname;
    char relbuffer[32], symbuffer[32];

    type      = bfd_get_16 (abfd, rel);
    relsecndx = bfd_get_16 (abfd, rel+2);
    reloffset = bfd_get_16 (abfd, rel+4);
    symsecndx = bfd_get_16 (abfd, rel+6);

    relsec = symsec = NULL;
    for (sec = abfd->sections; sec; sec = sec->next) {
      if (sec->index == relsecndx)  relsec = sec;
      if (sec->index == symsecndx)  symsec = sec;
      }

    sprintf (relbuffer, "[%d?]", (int) relsecndx);
    relsecname = (relsec)? bfd_section_name (abfd, relsec) : relbuffer;

    sprintf (symbuffer, "[%d?]", (int) symsecndx);
    symsecname = (symsec)? bfd_section_name (abfd, symsec) : symbuffer;

    ewhere ("%s+0x%04x", relsecname, reloffset);

    if (!relsec || resid_from_secndx[relsecndx] != 0) {
      einfo (E_FILEWHERE|E_WARNING,
	     "reloc in non-data section `%s'", relsecname);
      continue;
      }

    reloffset += bfd_section_vma (abfd, relsec);

    if (reloffset > datares_size - RELOC_SIZE) {
      einfo (E_FILEWHERE|E_WARNING, "reloc location out of range");
      continue;
      }

    if (!symsec || resid_from_secndx[symsecndx] == UNSPECIFIED_RESID) {
      einfo (E_FILEWHERE|E_WARNING,
	     "reloc relative to strange section `%s'", symsecname);
      continue;
      }

    switch (type) {
    case R_RELLONG: {
      unsigned long value = (get_long (datares + reloffset)
			     + bfd_section_vma (abfd, symsec));
      unsigned long symresid = resid_from_secndx[symsecndx];
      unsigned int prevoffset = get_short (reloc_heads + 2 * symresid);
      set_short (&datares[reloffset], prevoffset);
      set_short (&datares[reloffset+2], value);
      set_short (reloc_heads + 2 * symresid, reloffset);
      }
      break;

    default:
      einfo (E_FILEWHERE|E_WARNING, "unknown reloc type 0x%x", type);
      continue;
      }

    nrelocs++;
    }

  free (relocs);

  return nrelocs;
  }


/* Emit the whole block as a series of literal runs.  */
static unsigned char *
emit_literals (unsigned char *out,
	       const unsigned char *p, const unsigned char *lim) {
  while (p < lim) {
    int len = lim - p;
    if (len > 128)  len = 128;
    *out++ = 0x7f + len;
    memcpy (out, p, len);
    out += len;
    p += len;
    }

  return out;
  }

/* This greedy algorithm isn't always optimal (for example, consider 65 zeros
   followed by 128 random bytes -- and therefore there may be cases where it
   actually makes a significant difference).  But hopefully it's good
   enough.  */
static unsigned char *
compress_runs (unsigned char *out,
	       const unsigned char *in, const unsigned char *inlim) {
  while (in < inlim) {
    const unsigned char *copy_in = in;

    /* Scan until the data runs out or we find a good run.  */
    while (! (in >= inlim
	      || (in+1 < inlim && (*in == 0 || *in == 0xff) && *in == in[1])
	      || (in+2 < inlim && *in == in[1] && *in == in[2])))
      in++;

    out = emit_literals (out, copy_in, in);

    if (in < inlim) {
      int len, maxlen;

      for (len = 0; in + len < inlim && in[len] == *in; len++)
	;

      /* Only emit one run at a time, since the residue beyond maxlen may not
         be long enough to form a worthwhile run on its own.  */
      maxlen = (*in == 0)? 64 : (*in == 0xff)? 16 : 33;
      if (len > maxlen)  len = maxlen;

      if (*in == 0)  *out++ = 0x3f + len;
      else if (*in == 0xff)  *out++ = 0x0f + len;
      else  *out++ = 0x1e + len, *out++ = *in;

      in += len;
      }
    }

  return out;
  }


static void *
mem_A9F000xxxxxx00xx (const void *buf, size_t buflen) {
  const char *s = static_cast<const char *>(buf);
  const char *slim = s + buflen;
  for (;
       (s = (const char *) memmem (s, slim-s - 5, "\xA9\xF0\x00", 3)) != NULL;
       s += 3)
    if (s[6] == 0x00)
      return const_cast<char *>(s);

  return NULL;
  }

static unsigned char *
compress_patterns (unsigned char *out,
		   const unsigned char *in, const unsigned char *inlim) {
  const unsigned char *s;

  if ((s = (const unsigned char *)
		mem_A9F000xxxxxx00xx (in, inlim-in)) != NULL) {
    out = compress_patterns (out, in, s);
    if (s[3] == 0x00)
      *out++ = 0x03, *out++ = s[4], *out++ = s[5], *out++ = s[7];
    else
      *out++ = 0x04, *out++ = s[3], *out++ = s[4], *out++ = s[5], *out++ = s[7];
    out = compress_patterns (out, s + 8, inlim);
    }
  else if ((s = (const unsigned char *)
		     memmem (in, inlim-in - 3,
			     "\x00\x00\x00\x00\xFF", 5)) != NULL) {
    out = compress_patterns (out, in, s);
    if (s[5] == 0xFF)
      *out++ = 0x01, *out++ = s[6], *out++ = s[7];
    else
      *out++ = 0x02, *out++ = s[5], *out++ = s[6], *out++ = s[7];
    out = compress_patterns (out, s + 8, inlim);
    }
  else
    out = compress_runs (out, in, inlim);

  return out;
  }


static unsigned char *
compress_data (unsigned char *datap, const unsigned char *raw_end,
	       const unsigned char *rawp, const unsigned char *rawlim,
	       int compression) {

  set_long (datap, rawp - raw_end);
  datap += 4;

  switch (compression % 4) {
  case 0:
    datap = emit_literals (datap, rawp, rawlim);
    break;

  case 1:
    datap = compress_runs (datap, rawp, rawlim);
    break;

  default:
    datap = compress_patterns (datap, rawp, rawlim);
    break;
    }

  *datap++ = '\0';
  return datap;
  }


static void
find_maximal_zero_run (unsigned char **zeropp, unsigned char **zerolimp,
		       unsigned char *p, unsigned char *lim) {
  *zeropp = *zerolimp = lim;

  while (p < lim && (p = (unsigned char *) memchr (p, 0, lim - p)) != NULL) {
    unsigned char *zero = p;
    while (p < lim && *p == 0)  p++;
    if (p - zero > *zerolimp - *zeropp)
      *zeropp = zero, *zerolimp = p;
    }
  }


static pfd_data
make_data_and_rloc (int* nrelocsp, pfd_data& rloc_res, int nchains,
		    unsigned long resid_from_secndx[], bfd* abfd,
		    const binary_file_info& info, asection* data_sec,
		    size_t data_size, size_t total_data_size) {
  bfd_byte* raw_data = (bfd_byte*) xmalloc (total_data_size);
  bfd_byte* raw_end = raw_data + total_data_size;
  if (!bfd_get_section_contents (abfd, data_sec, raw_data, 0, data_size))
    einfo (E_FILE, "can't read `.data' section");

  *nrelocsp = make_reloc_chains (rloc_res, nchains, resid_from_secndx,
				 abfd, raw_data, data_size);

  pfd_data data_res (128 + 33 * (data_size / 32));
  unsigned char* data_res_data = data_res.writable_contents();
  unsigned char* datap = data_res_data + 4;

  if (info.data_compression >= 4) {
    unsigned char *Lp, *Llim, *Mp, *Mlim, *Rp, *Rlim;
    unsigned char *Lzerop, *Lzerolim, *Rzerop, *Rzerolim;

    Lp = raw_data;
    Rlim = raw_data + data_size;

    /* Ignore leading and trailing runs of zeros.  */

    while (Lp < Rlim && *Lp == 0)  Lp++;
    while (Rlim > Lp && Rlim[-1] == 0)  Rlim--;

    /* Now find the two longest internal runs of zeros.  */

    find_maximal_zero_run (&Llim, &Rp, Lp, Rlim);
    find_maximal_zero_run (&Lzerop, &Lzerolim, Lp, Llim);
    find_maximal_zero_run (&Rzerop, &Rzerolim, Rp, Rlim);

    if (Lzerolim - Lzerop > Rzerolim - Rzerop)
      Mp = Lzerolim, Mlim = Llim, Llim = Lzerop;
    else
      Mp = Rp, Mlim = Rzerop, Rp = Rzerolim;

    datap = compress_data (datap, raw_end, Lp, Llim, info.data_compression);
    datap = compress_data (datap, raw_end, Mp, Mlim, info.data_compression);
    datap = compress_data (datap, raw_end, Rp, Rlim, info.data_compression);

    if (info.stats)
      info.stats->omitted_zeros = (Lp - raw_data) + (Mp - Llim) + (Rp - Mlim)
				  + (raw_data + data_size - Rlim);
    }
  else {
    datap = compress_data (datap, raw_end, raw_data, raw_data+data_size,
			   info.data_compression);
    datap = compress_data (datap, raw_end, raw_data, raw_data,
			   info.data_compression);
    datap = compress_data (datap, raw_end, raw_data, raw_data,
			   info.data_compression);
    if (info.stats)
      info.stats->omitted_zeros = 0;
    }

  /* 6 longs of 0 because we don't know the format of the standard Palm OS
     relocation tables.  They wouldn't be much use for our way of handling
     multiple code resources anyway.  */

  set_long (datap+0, 0);
  set_long (datap+4, 0);
  set_long (datap+8, 0);
  datap += 12;

  /* Offset of the CODE 1 xrefs.  */
  set_long (data_res_data, datap - data_res_data);

  set_long (datap+0, 0);
  set_long (datap+4, 0);
  set_long (datap+8, 0);
  datap += 12;

  data_res.set_length (datap - data_res_data);
  return data_res;
  }

void
init_binary() {
  bfd_init();
  }

void
process_binary_file (pfd& outpfd, char* fname, const binary_file_info& info) {
  bfd* abfd = bfd_openr (fname, NULL);

  filename = fname;

  if (!abfd || !bfd_check_format (abfd, bfd_object)) {
    einfo (E_NOFILE, "can't open `%s': %s",
	   fname, bfd_errmsg (bfd_get_error ()));
    return;
    }

  asection* data_sec = bfd_get_section_by_name (abfd, ".data");
  asection* bss_sec  = bfd_get_section_by_name (abfd, ".bss");

  size_t data_size = bfd_section_size (abfd, data_sec);
  size_t bss_size  = bfd_section_size (abfd, bss_sec);

  /* Round up to the next longword boundary.  */
  size_t total_data_size = (data_size + bss_size + 3) & ~3;

  if (info.kind == DK_APPLICATION) {
    outpfd.attach (make_code0 (total_data_size), "code", 0, fname);
    outpfd.attach (make_pref(), "pref", 0, fname);
    }

  unsigned int nsections = bfd_count_sections (abfd);
  unsigned long* resid_from_secndx = new unsigned long[nsections];

  for (unsigned int i = 0; i < nsections; i++)
    resid_from_secndx[i] = UNSPECIFIED_RESID;

  resid_from_secndx[data_sec->index] = 0;
  resid_from_secndx[bss_sec->index] = 0;

  asection* text_sec = bfd_get_section_by_name (abfd, ".text");
  resid_from_secndx[text_sec->index] = 1;

  char* maintype;
  unsigned long mainid;

  switch (info.kind) {
  case DK_APPLICATION:
    maintype = "code", mainid = 1;
    break;
  case DK_GLIB:
    maintype = "GLib", mainid = 0;
    break;
  case DK_SYSLIB:
    maintype = "libr", mainid = 0;
    break;
  default:
    maintype = "code", mainid = 1000;
    break;
    }

  if (info.maincode_id != UNSPECIFIED_RESID)
    mainid = info.maincode_id;

  outpfd.attach (make_code (abfd, text_sec, info.kind, true),
		 maintype, mainid, fname);

  int nchains;
  char* name;
  unsigned long id;
  for (nchains = 2; info.next_coderes (&name, &id); nchains++) {
    asection* sec = bfd_get_section_by_name (abfd, name);
    if (sec) {
      resid_from_secndx[sec->index] = id;
      outpfd.attach (make_code (abfd, sec, info.kind, false),
		     "code", id, fname);
      }
    else
      einfo (E_FILE, "unknown code section `%s'", name);
    }
    
  for (asection* sec = abfd->sections; sec; sec = sec->next)
    if (bfd_get_section_flags (abfd, sec) & SEC_CODE)
      if (resid_from_secndx[sec->index] == UNSPECIFIED_RESID) {
	ewhere ("%s", bfd_section_name (abfd, sec));
        einfo (E_FILEWHERE|E_WARNING, "spurious code section ignored");
	}

  if (info.emit_data) {
    int nrelocs;
    pfd_data rloc_res (2 * nchains);
    outpfd.attach (make_data_and_rloc (&nrelocs, rloc_res, nchains,
				       resid_from_secndx, abfd, info,
				       data_sec, data_size, total_data_size),
		   "data", 0, fname);

    if (nrelocs > 0 || info.force_rloc)
      outpfd.attach (rloc_res, "rloc", 0, fname);
    }
  else if (total_data_size > 0)
    einfo (E_FILE|E_WARNING, "global data ignored");

  if (info.stats)
    info.stats->data_size = data_size;

  bfd_close (abfd);
  }
