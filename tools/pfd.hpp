/* pfd.hpp: read and write PalmOS .pdb / .prc files.

   Copyright (c) 1998 by John Marshall.
   <jmarshall@acm.org>

   This is free software, under the GNU General Public Licence v2 or greater.
 */

#ifndef _PFD_HPP_
#define _PFD_HPP_

struct pfd_datablock {
  unsigned int refcount;
  unsigned char data[1];
  };

class pfd;

class pfd_data {
public:
  pfd_data (size_t length);
  pfd_data (const pfd_data& rhs);
  pfd_data& operator= (const pfd_data& rhs);
  ~pfd_data();

  const unsigned char* contents() const { return block->data + offset; }
  unsigned char* writable_contents();
  size_t length() const { return len; }
  void set_length (size_t newlen);

private:
  friend class pfd;

  pfd_data (pfd_datablock* block, size_t offset, size_t len);

  pfd_datablock* block;
  size_t offset, len;
  };

class pfd_entry: public pfd_data {
public:
  const char* type() const { return type_; }
  unsigned long id() const { return id_; }
  unsigned short attr() const { return attr_; }
  void* userdata() const { return userdata_; }

private:
  friend class pfd;
  pfd_entry (const pfd_data& data, const char* type, unsigned long id,
	     unsigned short attr, void* userdata);
  ~pfd_entry();
  /*
  pfd_entry (pfd_datablock* block, size_t offset, size_t len, const char* type,
	     unsigned long id, unsigned short attr, void* userdata);
  */

  pfd_entry (const pfd_entry& rhs);  // Not defined
  pfd_entry& operator= (const pfd_entry& rhs);  // Not defined

  char type_[4];
  unsigned long id_;
  unsigned short attr_;
  void* userdata_;

  pfd_entry* next;
  pfd_entry* prev;
  };

class pfd {
public:
  pfd();
  ~pfd();

  enum pfd_kind { record_db, resource_db };
  void write (pfd_kind kind, const char* fname);

  pfd_data* appinfo() const { return special[0]; }
  bool attach_appinfo (const pfd_data& data)
    { return attach_special (0, data); }
  void remove_appinfo() { remove_special (0); }

  pfd_data* sortinfo() const { return special[1]; }
  bool attach_sortinfo (const pfd_data& data)
    { return attach_special (1, data); }
  void remove_sortinfo() { remove_special (1); }

  int size() const { return nentries; }

  pfd_entry* operator[] (int index) const;
  pfd_entry* succ (const pfd_entry* e) const { return e->next; }
  pfd_entry* pred (const pfd_entry* e) const { return e->prev; }
  pfd_entry* find (const char* type, unsigned long id) const;
  pfd_entry* find (unsigned long id) const { return find ("", id); }

  bool attach (const pfd_data& data, const char* type, unsigned long id,
	       void* userdata = NULL)
    { return attach_before (NULL, data, type, id, userdata); }

  bool attach (const pfd_data& data, unsigned long id, unsigned short attr,
	       void* userdata = NULL)
    { return attach_before (NULL, data, id, attr, userdata); }

  bool attach_before (pfd_entry* entry, const pfd_data& data,
		      const char* type, unsigned long id,
		      void* userdata = NULL)
    { return attach_internal (entry, data, type, id, 0, userdata); }

  bool attach_before (pfd_entry* entry, const pfd_data& data,
		      unsigned long id, unsigned short attr,
		      void* userdata = NULL)
    { return attach_internal (entry, data, "", id, attr, userdata); }

  void remove (pfd_entry* entry);

private:
  void init (int size);
  pfd (const pfd& rhs);  // Not defined
  pfd& operator= (const pfd& rhs);  // Not defined

  bool attach_internal (pfd_entry* entry, const pfd_data& data,
			const char* type, unsigned long id, unsigned short attr,
			void* userdata);

  bool attach_special (int which, const pfd_data& data);
  void remove_special (int which);

  pfd_entry*& lookup (const char* type, unsigned long id) const;
  void rehash (unsigned int size);

  int nentries;
  pfd_entry* first;
  pfd_entry* last;

  pfd_data* special[2];

  pfd_entry** tab;
  unsigned int tab_free, tab_capacity;

  // This would be static but for fears about portability of global
  // objects.  :-)
  pfd_entry* TOMBSTONE;
  };

/* Until pi-macros.h uses this convention too.  */
#ifndef _PILOT_MACROS_H_

#ifndef _PILOT_LINK_SWAPPING_MACROS_
#define _PILOT_LINK_SWAPPING_MACROS_

inline unsigned long get_long(const void *buf) 
{
     unsigned char *ptr = (unsigned char *) buf;

     return (*ptr << 24) | (*(++ptr) << 16) | (*(++ptr) << 8) | *(++ptr);
}

inline signed long get_slong(const void *buf)
{
     unsigned long val = get_long(buf);
     if (val > 0x7FFFFFFF)
         return ((signed long)(val & 0x7FFFFFFF)) - 0x80000000;
     else
         return val;
}

inline unsigned long get_treble(const void *buf) 
{
     unsigned char *ptr = (unsigned char *) buf;

     return (*ptr << 16) | (*(++ptr) << 8) | *(++ptr);
}

inline signed long get_streble(const void *buf)
{
     unsigned long val = get_treble(buf);
     if (val > 0x7FFFFF)
         return ((signed long)(val & 0x7FFFFF)) - 0x800000;
     else
         return val;
}

inline int get_short(const void *buf) 
{
     unsigned char *ptr = (unsigned char *) buf;

     return (*ptr << 8) | *(++ptr);
}

inline signed short get_sshort(const void *buf)
{
     unsigned short val = get_short(buf);
     if (val > 0x7FFF)
         return ((signed short)(val & 0x7FFF)) - 0x8000;
     else
         return val;
}

inline int get_byte(const void *buf) 
{
     return *((unsigned char *) buf);
}

inline signed char get_sbyte(const void *buf)
{
     unsigned char val = get_byte(buf);
     if (val > 0x7F)
         return ((signed char)(val & 0x7F)) - 0x80;
     else
         return val;
}

inline void set_long(void *buf, const unsigned long val) 
{
     unsigned char *ptr = (unsigned char *) buf;

     *ptr = (val >> 24) & 0xff;
     *(++ptr) = (val >> 16) & 0xff;
     *(++ptr) = (val >> 8) & 0xff;
     *(++ptr) = val & 0xff;
}

inline void set_slong(void *buf, const signed long val) 
{
     unsigned long uval;
     
     if (val < 0) {
         uval = (val + 0x80000000);
         uval |= 0x80000000;
     } else
         uval = val;
     set_long(buf, uval);
}

inline void set_treble(void *buf, const unsigned long val) 
{
     unsigned char *ptr = (unsigned char *) buf;
     
     *ptr = (val >> 16) & 0xff;
     *(++ptr) = (val >> 8) & 0xff;
     *(++ptr) = val & 0xff;
}

inline void set_streble(void *buf, const signed long val) 
{
     unsigned long uval;
     
     if (val < 0) {
         uval = (val + 0x800000);
         uval |= 0x800000;
     } else
         uval = val;
     set_treble(buf, uval);
}

inline void set_short(void *buf, const int val) 
{
     unsigned char *ptr = (unsigned char *) buf;

     *ptr = (val >> 8) & 0xff;
     *(++ptr) = val & 0xff;
}

inline void set_sshort(void *buf, const signed short val) 
{
     unsigned short uval;
     
     if (val < 0) {
         uval = (val + 0x8000);
         uval |= 0x8000;
     } else
         uval = val;
     set_treble(buf, uval);
}

inline void set_byte(void *buf, const int val) 
{
     *((unsigned char *)buf) = val;
}

inline void set_sbyte(void *buf, const signed char val) 
{
     unsigned char uval;
     
     if (val < 0) {
         uval = (val + 0x80);
         uval |= 0x80;
     } else
         uval = val;
     set_byte(buf, uval);
}

#endif
#endif

#endif
