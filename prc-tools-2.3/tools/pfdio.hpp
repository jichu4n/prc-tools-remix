/* pfdio.hpp: ye olde endianness correcting I/O routines.

   Copyright (c) 1999 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.

   This is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.  */

#ifndef PFDIO_HPP
#define PFDIO_HPP

/* These'll mostly go away when I finally finish the structure pictures,
   because:  ENDIANNESS IS NOT THE ONLY PROBLEM!  */

inline unsigned char
get_byte (const unsigned char*& s) {
  return *s++;
  }

inline signed char
get_sbyte (const unsigned char*& s) {
  unsigned char u = get_byte (s);
  if (u >= 0x80) {
    signed char v = u & 0x7f;
    return v - 0x80;
    }
  else
    return u;
  }

inline unsigned int
get_word (const unsigned char*& s) {
  unsigned int v = *s++;
  v = (v << 8) | *s++;
  return v;
  }

inline signed int
get_sword (const unsigned char*& s) {
  unsigned int u = get_word (s);
  if (u >= 0x8000) {
    signed int v = u & 0x7fff;
    return v - 0x8000;
    }
  else
    return u;
  }

inline unsigned long
get_long (const unsigned char*& s) {
  unsigned int v = *s++;
  v = (v << 8) | *s++;
  v = (v << 8) | *s++;
  v = (v << 8) | *s++;
  return v;
  }

inline signed long
get_slong (const unsigned char*& s) {
  unsigned long u = get_long (s);
  if (u >= 0x80000000) {
    signed long v = u & 0x7fffffff;
    return v - 0x80000000;
    }
  else
    return u;
  }

inline void
put_byte (unsigned char*& s, unsigned char v) {
  *s++ = v;
  }

inline void
put_sbyte (unsigned char*& s, signed char v) {
  unsigned char u = (v < 0)? ((v + 0x80) | 0x80) : v;
  put_byte (s, u);
  }

inline void
put_word (unsigned char*& s, unsigned int v) {
  *s++ = v >> 8, *s++ = v;
  }

inline void
put_sword (unsigned char*& s, signed int v) {
  unsigned int u = (v < 0)? ((v + 0x8000) | 0x8000) : v;
  put_word (s, u);
  }

inline void
put_long (unsigned char*& s, unsigned long v) {
  *s++ = v >> 24, *s++ = v >> 16, *s++ = v >> 8, *s++ = v;
  }

inline void
put_slong (unsigned char*& s, signed long v) {
  unsigned long u = (v < 0)? ((v + 0x80000000) | 0x80000000) : v;
  put_long (s, u);
  }

#endif
