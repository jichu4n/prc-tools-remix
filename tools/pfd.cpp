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



PalmOSDatabase::PalmOSDatabase (bool res0) : gap (2), resource (res0) {
  memset (gap.writable_contents(), 0, gap.size());
  }

PalmOSDatabase::~PalmOSDatabase() {
  }

static inline bool
write_datablock (FILE* f, const Datablock& block) {
  return fwrite (block.contents(), 1, block.size(), f) == size_t (block.size());
  }

bool
PalmOSDatabase::write (FILE* f) const {
  unsigned short attributes = 0;
  if (resource)		attributes |= 0x0001;
  if (readonly)		attributes |= 0x0002;
  if (appinfo_dirty)	attributes |= 0x0004;
  if (backup)		attributes |= 0x0008;
  if (ok_to_install_newer)  attributes |= 0x0010;
  if (reset_after_install)  attributes |= 0x0020;
  if (copy_prevention)	attributes |= 0x0040;
  if (stream)		attributes |= 0x0080;
  if (hidden)		attributes |= 0x0100;
  if (launchable_data)	attributes |= 0x0200;

  unsigned char header[78];
  unsigned char* s = header;

  unsigned long offset = sizeof header + ((resource)? 10 : 8) * dbsize();

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
  put_word (s, dbsize());

  return fwrite (header, 1, sizeof header, f) == sizeof header
      && write_directory (f, offset)
      && write_datablock (f, gap)
      && write_datablock (f, appinfo)
      && write_datablock (f, sortinfo)
      && write_data (f, offset);
  }



ResourceDatabase::ResourceDatabase() : PalmOSDatabase (true) {
  }

ResourceDatabase::~ResourceDatabase() {
  }

bool
ResourceDatabase::write_directory (FILE* f, unsigned long& offset) const {

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
ResourceDatabase::write_data (FILE* f, unsigned long& offset) const {
  for (ResourceMap::const_iterator it = begin(); it != end(); ++it) {
    if (!write_datablock (f, (*it).second))  return false;
    offset += (*it).second.size();
    }

  return true;
  }



RecordDatabase::RecordDatabase() : PalmOSDatabase (false) {
  }

RecordDatabase::~RecordDatabase() {
  }

bool
RecordDatabase::write_directory (FILE* f, unsigned long& offset) const {
  for (RecordMap::const_iterator it = begin(); it != end(); ++it) {
    unsigned char buffer[8];
    unsigned char* s = buffer;

    unsigned char attributes = (*it).second.category;
    if ((*it).second.deletable)	attributes |= 0x80;
    if ((*it).second.dirty)	attributes |= 0x40;
    if ((*it).second.busy)	attributes |= 0x20;
    if ((*it).second.secret)	attributes |= 0x10;

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
RecordDatabase::write_data (FILE* f, unsigned long& offset) const {
  for (RecordMap::const_iterator it = begin(); it != end(); ++it) {
    if (!write_datablock (f, (*it).second))  return false;
    offset += (*it).second.size();
    }

  return true;
  }
