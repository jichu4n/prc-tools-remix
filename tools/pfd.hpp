/* pfd.hpp: read/write PRC and PDB files.

   Copyright (c) 1999 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#ifndef PFD_HPP
#define PFD_HPP

#include <map>

#include <string.h>
#include <stdio.h>

#include "pfdheader.h"

class Datablock {
public:
  Datablock (long size = 0);
  Datablock (const Datablock& rhs);
  Datablock& operator= (const Datablock& rhs);
  ~Datablock ();

  long size () const { return len_; }
  const unsigned char* contents () const { return b_->data_ + off_; }
  unsigned char* writable_contents ();

  Datablock operator() (long off, long len) const;

private:
  Datablock (const Datablock& rhs, long off0, long len0);
  Datablock dup (long off, long len) const;

  class block {
  public:
    block (long size) : count_ (0), data_ (new unsigned char[size]) { }
    ~block () { delete [] data_; }
    unsigned int count_;
    unsigned char* data_;
    };

  block* b_;
  long off_, len_;
  };


typedef unsigned long RecKey;

struct Record: public Datablock {
  bool deletable, dirty, busy, secret;
  unsigned int category;
  };


struct ResKey {
  char type[4];
  unsigned int id;
  ResKey () {}
  ResKey (const char* type0, unsigned int id0) {
    strncpy (type, type0, 4);
    id = id0;
    }
  };

inline bool
operator< (const ResKey& a, const ResKey& b) {
  int str = strncmp (a.type, b.type, 4);
  return str < 0 || (str == 0 && a.id < b.id);
  }


typedef map<ResKey, Datablock> ResourceMap;
typedef map<RecKey, Record> RecordMap;

class PalmOSDatabase: public DatabaseHeader {
public:
  bool write (FILE* f) const;
  virtual ~PalmOSDatabase();

  Datablock gap, appinfo, sortinfo;

protected:
  PalmOSDatabase (bool res0);
  PalmOSDatabase (bool res0, const Datablock& block);
  void read_header (const Datablock& block, long info, long infolim);

private:
  virtual unsigned long directory_size() const = 0;
  virtual bool write_directory (FILE* f, unsigned long& off) const = 0;
  virtual bool write_data (FILE* f) const = 0;

  const bool resource;
  };

class ResourceDatabase: public PalmOSDatabase, public ResourceMap {
public:
  ResourceDatabase();
  ResourceDatabase (const Datablock& block);
  virtual ~ResourceDatabase();

private:
  virtual unsigned long directory_size() const {return directory_size (size());}
  static  unsigned long directory_size (unsigned int n);
  virtual bool write_directory (FILE* f, unsigned long& off) const;
  virtual bool write_data (FILE* f) const;
  };

class RecordDatabase: public PalmOSDatabase, public RecordMap {
public:
  RecordDatabase();
  RecordDatabase (const Datablock& block);
  virtual ~RecordDatabase();

private:
  virtual unsigned long directory_size() const {return directory_size (size());}
  static  unsigned long directory_size (unsigned int n);
  virtual bool write_directory (FILE* f, unsigned long& off) const;
  virtual bool write_data (FILE* f) const;
  };

#endif
