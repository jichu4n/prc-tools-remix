/* pfd.cpp: read and write PalmOS .pdb / .prc files.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.
 */

#include <stdlib.h>
#include <string.h>

#include "pfd.hpp"

static pfd_datablock*
alloc_block (size_t length) {
  pfd_datablock* p = (pfd_datablock*) malloc (sizeof (unsigned int) + length);
  if (!p)  abort();
  p->refcount = 1;
  return p;
  }

static void
release_block (pfd_datablock* p) {
  if (--p->refcount == 0)
    free (p);
  }

pfd_data::pfd_data (size_t len0)
  : block (alloc_block (len0)), offset (0), len (len0) {
  }

pfd_data::pfd_data (const pfd_data& rhs)
  : block (rhs.block), offset (rhs.offset), len (rhs.len) {
  block->refcount++;
  }

pfd_data::pfd_data (pfd_datablock* block0, size_t offset0, size_t len0)
  : block (block0), offset (offset0), len (len0) {
  block->refcount++;
  }

pfd_data& pfd_data::operator= (const pfd_data& rhs) {
  if (this != &rhs) {
    release_block (block);
    block = rhs.block, offset = rhs.offset, len = rhs.len;
    block->refcount++;
    }

  return *this;
  }

pfd_data::~pfd_data() {
  release_block (block);
  }

unsigned char*
pfd_data::writable_contents() {
  if (block->refcount > 1) {
    pfd_datablock* newb = alloc_block (len);
    memcpy (newb->data, contents(), len);
    release_block (block);
    block = newb;
    offset = 0;
    }

  return block->data + offset;
  }

void
pfd_data::set_length (size_t newlen) {
  if (newlen > len) {
    pfd_datablock* newb = alloc_block (newlen);
    memcpy (newb->data, contents(), len);
    release_block (block);
    block = newb;
    offset = 0;
    }

  len = newlen;
  }


pfd_entry::pfd_entry (const pfd_data& data, const char* type, unsigned long id,
		      unsigned short attr, void* userdata)
  : pfd_data (data), id_ (id), attr_ (attr), userdata_ (userdata) {
  strncpy (type_, type, 4);
  }

pfd_entry::~pfd_entry() {
  }


pfd::pfd() {
  init (37);
  }

void
pfd::init (int size) {
  nentries = 0;
  first = last = NULL;

  special[0] = special[1] = NULL;

  // Such a kerfuffle to make another sentinel value different from NULL.
  TOMBSTONE = new pfd_entry (pfd_data (0), "", ~0ul, 0, NULL);

  tab = NULL;
  rehash (size);
  }

pfd::~pfd() {
  pfd_entry* e = first;
  while (e) {
    pfd_entry* tmp = e;
    e = e->next;
    delete tmp;
    }

  delete special[0];
  delete special[1];

  delete [] tab;
  delete TOMBSTONE;
  }


static unsigned int
hash1 (const char* k1, unsigned long k2) {
  return 0;
  }

static unsigned int
hash2 (const char* k1, unsigned long k2) {
  return 1;
  }

// Look up the key (TYPE, ID) in the hash table, and return a reference
// to the slot for that key.  The slot will contain a pointer to the entry,
// or will contain NULL if the key is not present in the table.

#include <stdio.h>

pfd_entry*&
pfd::lookup (const char* type, unsigned long id) const {
  unsigned int h, dh;

  for (h = hash1 (type, id) % tab_capacity, dh = hash2 (type, id);
       tab[h] && (tab[h]->id() != id || strncmp (tab[h]->type(), type, 4) != 0);
       h = (h + dh) % tab_capacity)
    ;

  return tab[h];
  }


// Create a new hash table with capacity for the given SIZE.  Populate
// it from the linked list of pfd_entries.  (Assumes that the table is
// definitely large enough for the SIZE, including slack space.)

void
pfd::rehash (unsigned int size) {
  delete [] tab;

  tab_capacity = ((7 * size) / 5 + 37) | 1;

  tab = new pfd_entry*[tab_capacity];

  tab_free = tab_capacity;
  for (pfd_entry* e = first; e; e = e->next) {
    lookup (e->type(), e->id()) = e;
    tab_free--;
    }
  }


pfd_entry*
pfd::find (const char* type, unsigned long id) const {
  return lookup (type, id);
  }

bool
pfd::attach_internal (pfd_entry* succe, const pfd_data& data,
		      const char* type, unsigned long id, unsigned short attr,
		      void* userdata) {
  pfd_entry*& newe = lookup (type, id);

  if (newe) {
    //if (dup_fn)  (*dup_fn)(type, id, (*newe)->userdata, userdata);
    return false;
    }

  newe = new pfd_entry (data, type, id, attr, userdata);
  tab_free--;

  pfd_entry* preve = (succe)? succe->prev : last;
  newe->next = succe;
  newe->prev = preve;
  if (preve)  preve->next = newe;
  else  first = newe;
  if (succe)  succe->prev = newe;
  else  last = newe;
  nentries++;

  if (tab_free < tab_capacity / 5)
    rehash ((3 * nentries) / 2);

  return true;
  }

void
pfd::remove (pfd_entry* entry) {
  pfd_entry*& e = lookup (entry->type(), entry->id());
  if (e) {
    if (e->prev)  e->prev->next = e->next;
    else  first = e->next;
    if (e->next)  e->next->prev = e->prev;
    else  last = e->prev;
    nentries--;

    delete e;

    e = TOMBSTONE;
    }
  }


pfd_entry*
pfd::operator[] (int index) const {
  pfd_entry* e;

  if (index >= nentries || index < -nentries)
    e = NULL;
  else if (index >= 0)
    for (e = first; index--; e = e->next)
      ;
  else
    for (e = last; ++index; e = e->prev)
      ;

  return e;
  }


bool
pfd::attach_special (int which, const pfd_data& data) {
  if (special[which])
    return false;
  else {
    special[which] = new pfd_data (data);
    return true;
    }
  }
 
void
pfd::remove_special (int which) {
  if (special[which]) {
    delete special[which];
    special[which] = NULL;
    }
  }
