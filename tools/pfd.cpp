/* pfd.cpp: read/write PRC and PDB files.

   Copyright (c) 1999 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#include "pfd.hpp"
#include "pfdio.hpp"

#include <algorithm>
#include <string.h>

Datablock::Datablock (long len0)
  : b_ (new block (len0)), off_ (0), len_ (len0) { b_->count_++; }

Datablock::Datablock (const Datablock& rhs)
  : b_ (rhs.b_), off_ (rhs.off_), len_ (rhs.len_) { b_->count_++; }

Datablock::Datablock (const Datablock& rhs, long off0, long len0)
  : b_ (rhs.b_), off_ (off0), len_ (len0) { b_->count_++; }

Datablock&
Datablock::operator= (const Datablock& rhs) {
  rhs.b_->count_++;
  if (--b_->count_ == 0)  delete b_;
  b_ = rhs.b_, off_ = rhs.off_, len_ = rhs.len_;
  return *this;
  }

Datablock::~Datablock () {
  if (--b_->count_ == 0)  delete b_;
  }

unsigned char*
Datablock::writable_contents () {
  if (b_->count_ > 1)
    *this = dup (0, len_);

  return b_->data_ + off_;
  }

Datablock
Datablock::operator() (long off, long len) const {
  if (off >= 0 && off + len <= len_)
    return Datablock (*this, off_ + off, len);
  else
    return dup (off, len);
  }

Datablock
Datablock::dup (long off, long len) const {
  long off__ = off_, len__ = len_;

  // Clip the existing interval against the desired boundaries:

  if (off > 0) {
    off__ += off;
    len__ -= off;
    off = 0;
    }

  if (off + len < len__)
    len__ = off + len;

  // And build a new interval:

  Datablock lhs (len);

  memset (lhs.b_->data_, '\0', -off);
  memcpy (lhs.b_->data_ + -off, b_->data_ + off__, len__);
  memset (lhs.b_->data_ + -off + len__, '\0', len - -off - len__);

  return lhs;
  }


// Bit masks for the header attributes from DataMgr.h

static const unsigned int resource_mask		   = 0x0001;
static const unsigned int readonly_mask		   = 0x0002;
static const unsigned int appinfo_dirty_mask	   = 0x0004;
static const unsigned int backup_mask		   = 0x0008;
static const unsigned int ok_to_install_newer_mask = 0x0010;
static const unsigned int reset_after_install_mask = 0x0020;
static const unsigned int copy_prevention_mask	   = 0x0040;
static const unsigned int stream_mask		   = 0x0080;
static const unsigned int hidden_mask		   = 0x0100;
static const unsigned int launchable_data_mask	   = 0x0200;

// We consider a database header to include everything up to the "next
// directory offset" (which is always 0), but not the entry count, which
// is more usefully considered part of the directory.

static const long header_size = 76;

// This constructor is for new databases about to be written out.  We create
// a short empty gap because all the other PRC/PDB writers seem to do so.

PalmOSDatabase::PalmOSDatabase (bool res0) : gap (2), resource (res0) {
  memset (gap.writable_contents(), 0, gap.size());
  }

// This constructor is for existing databases being read in.  We do a minimal
// check that it's in a valid format.  (FIXME: What else could we check?)

PalmOSDatabase::PalmOSDatabase (bool res0, const Datablock& block)
  : resource (res0) {
  if (block.size() < header_size + 2)
    throw "database header (or record count) is truncated";

  const unsigned char* s = block.contents();

  s += 32;  // Skip the name
  unsigned int attributes = get_word (s);
  bool file_resource = (attributes & resource_mask) != 0;

  if (resource && !file_resource)
    throw "database is not a resource database";
  else if (!resource && file_resource)
    throw "database is not a record database";
  }

PalmOSDatabase::~PalmOSDatabase() {
  }

void
PalmOSDatabase::read_header (const Datablock& block, long info, long infolim) {
  const unsigned char* s = block.contents();

  memcpy (name, s, 32), s += 32;

  unsigned int attributes = get_word (s);

  readonly		= (attributes & readonly_mask) != 0;
  appinfo_dirty		= (attributes & appinfo_dirty_mask) != 0;
  backup		= (attributes & backup_mask) != 0;
  ok_to_install_newer	= (attributes & ok_to_install_newer_mask) != 0;
  reset_after_install	= (attributes & reset_after_install_mask) != 0;
  copy_prevention	= (attributes & copy_prevention_mask) != 0;
  stream		= (attributes & stream_mask) != 0;
  hidden		= (attributes & hidden_mask) != 0;
  launchable_data	= (attributes & launchable_data_mask) != 0;

  version = get_word (s);
  tm_of_palmostime (&created, get_long (s));
  tm_of_palmostime (&modified, get_long (s));
  tm_of_palmostime (&backedup, get_long (s));
  modnum = get_long (s);
  long appinfo_offset = get_long (s);
  long sortinfo_offset = get_long (s);
  memcpy (type, s, 4), s += 4;
  memcpy (creator, s, 4), s += 4;
  uidseed = get_long (s);
  s += 4;

  if (info > infolim || info > block.size() || infolim > block.size())
    throw "corrupt 1";

  if (sortinfo_offset) {
    if (sortinfo_offset < info || sortinfo_offset >= infolim)
      throw "corrupt 2";
    sortinfo = block (sortinfo_offset, infolim - sortinfo_offset);
    infolim = sortinfo_offset;
    }

  if (appinfo_offset) {
    if (appinfo_offset < info || appinfo_offset >= infolim)
      throw "corrupt 3";
    appinfo = block (appinfo_offset, infolim - appinfo_offset);
    infolim = appinfo_offset;
    }

  if (infolim > info)
    gap = block (info, infolim - info);
  }


static inline bool
write_datablock (FILE* f, const Datablock& block) {
  return fwrite (block.contents(), 1, block.size(), f) == size_t (block.size());
  }

bool
PalmOSDatabase::write (FILE* f) const {
  unsigned int attributes = 0;
  if (resource)			attributes |= resource_mask;
  if (readonly)			attributes |= readonly_mask;
  if (appinfo_dirty)		attributes |= appinfo_dirty_mask;
  if (backup)			attributes |= backup_mask;
  if (ok_to_install_newer)	attributes |= ok_to_install_newer_mask;
  if (reset_after_install)	attributes |= reset_after_install_mask;
  if (copy_prevention)		attributes |= copy_prevention_mask;
  if (stream)			attributes |= stream_mask;
  if (hidden)			attributes |= hidden_mask;
  if (launchable_data)		attributes |= launchable_data_mask;

  unsigned char header[header_size];
  unsigned char* s = header;

  unsigned long offset = header_size + directory_size();

  memcpy (s, name, 32), s += 32;
  put_word (s, attributes);
  put_word (s, version);
  put_long (s, palmostime_of_tm (&created));
  put_long (s, palmostime_of_tm (&modified));
  put_long (s, palmostime_of_tm (&backedup));
  put_long (s, modnum);
  offset += gap.size();
  put_long (s, appinfo.size()? offset : 0);
  offset += appinfo.size();
  put_long (s, sortinfo.size()? offset : 0);
  offset += sortinfo.size();
  memcpy (s, type, 4), s += 4;
  memcpy (s, creator, 4), s += 4;
  put_long (s, uidseed);
  put_long (s, 0);

  return fwrite (header, 1, sizeof header, f) == sizeof header
      && write_directory (f, offset)
      && write_datablock (f, gap)
      && write_datablock (f, appinfo)
      && write_datablock (f, sortinfo)
      && write_data (f);
  }



ResourceDatabase::ResourceDatabase() : PalmOSDatabase (true) {
  }

ResourceDatabase::ResourceDatabase (const Datablock& block)
  : PalmOSDatabase (true, block) {
  const unsigned char* dir = block.contents() + header_size;
  unsigned int nrecs = get_word (dir);

  long entrystart = header_size + directory_size (nrecs);
  long entrylim = block.size();

  const unsigned char* d = block.contents() + entrystart;
  while ((d -= 10) >= dir) {
    const unsigned char* s = d;
    ResKey key;
    memcpy (key.type, s, 4), s += 4;
    key.id = get_word (s);
    long entry = get_long (s);

    if (entry < entrystart || entry > entrylim)
      throw "corrupt 4";

    insert (ResourceMap::value_type (key, block (entry, entrylim - entry)));
    entrylim = entry;
    }

  read_header (block, entrystart, entrylim);
  }

ResourceDatabase::~ResourceDatabase() {
  }

unsigned long
ResourceDatabase::directory_size (unsigned int n) {
  return 2 + 10 * n;
  }

bool
ResourceDatabase::write_directory (FILE* f, unsigned long& offset) const {
  { unsigned char buffer[2];
    unsigned char* s = buffer;
    put_word (s, size());
    if (fwrite (buffer, 1, sizeof buffer, f) != sizeof buffer)  return false;
    }

  for (ResourceMap::const_iterator it = begin(); it != end(); ++it) {
    unsigned char buffer[10];
    unsigned char* s = buffer;
    memcpy (s, (*it).first.type, 4), s += 4;
    put_word (s, (*it).first.id);
    put_long (s, offset);
    if (fwrite (buffer, 1, sizeof buffer, f) != sizeof buffer)  return false;
    offset += (*it).second.size();
    }

  return true;
  }

bool
ResourceDatabase::write_data (FILE* f) const {
  for (ResourceMap::const_iterator it = begin(); it != end(); ++it)
    if (!write_datablock (f, (*it).second))  return false;

  return true;
  }



// Bit masks for the record attributes from DataMgr.h

static const unsigned char category_mask  = 0x0f;
static const unsigned char secret_mask	  = 0x10;
static const unsigned char busy_mask	  = 0x20;
static const unsigned char dirty_mask	  = 0x40;
static const unsigned char deletable_mask = 0x80;

RecordDatabase::RecordDatabase() : PalmOSDatabase (false) {
  }

RecordDatabase::RecordDatabase (const Datablock& block)
  : PalmOSDatabase (false, block) {
  const unsigned char* dir = block.contents() + header_size;
  unsigned int nrecs = get_word (dir);

  long entrystart = header_size + directory_size (nrecs);
  long entrylim = block.size();

  const unsigned char* d = block.contents() + entrystart;
  while ((d -= 8) >= dir) {
    const unsigned char* s = d;
    long entry = get_long (s);
    RecKey key = get_long (s) & 0xfffffful;
    s -= 4;
    unsigned char attributes = get_byte (s);

    if (entry < entrystart || entry > entrylim)
      throw "corrupt 5";

    Record rec;
    static_cast<Datablock>(rec) = block (entry, entrylim - entry);
    rec.category  =  attributes & category_mask;
    rec.deletable = (attributes & deletable_mask) != 0;
    rec.dirty	  = (attributes & dirty_mask) != 0;
    rec.busy	  = (attributes & busy_mask) != 0;
    rec.secret	  = (attributes & secret_mask) != 0;

    insert (RecordMap::value_type (key, rec));
    entrylim = entry;
    }

  read_header (block, entrystart, entrylim);
  }

RecordDatabase::~RecordDatabase() {
  }

unsigned long
RecordDatabase::directory_size (unsigned int n) {
  return 2 + 8 * n;
  }

bool
RecordDatabase::write_directory (FILE* f, unsigned long& offset) const {
  { unsigned char buffer[2];
    unsigned char* s = buffer;
    put_word (s, size());
    if (fwrite (buffer, 1, sizeof buffer, f) != sizeof buffer)  return false;
    }

  for (RecordMap::const_iterator it = begin(); it != end(); ++it) {
    unsigned char buffer[8];
    unsigned char* s = buffer;

    unsigned char attributes = (*it).second.category & category_mask;
    if ((*it).second.deletable)	attributes |= deletable_mask;
    if ((*it).second.dirty)	attributes |= dirty_mask;
    if ((*it).second.busy)	attributes |= busy_mask;
    if ((*it).second.secret)	attributes |= secret_mask;

    put_long (s, offset);
    put_long (s, (*it).first);
    s -= 4;
    put_byte (s, attributes);
    if (fwrite (buffer, 1, sizeof buffer, f) != sizeof buffer)  return false;
    offset += (*it).second.size();
    }

  return true;
  }

bool
RecordDatabase::write_data (FILE* f) const {
  for (RecordMap::const_iterator it = begin(); it != end(); ++it)
    if (!write_datablock (f, (*it).second))  return false;

  return true;
  }
