/* m68k/stdlib.h: some inline versions optimized for m68k and Palm OS.

   Placed in the public domain by John Marshall.

   This file is part of prc-tools, but is not licensed in the same way
   as the majority of prc-tools.  The contents of this file are in the
   public domain.

   One particular consequence of this is that you may use these functions
   in Palm OS executables without concern that doing so by itself causes
   the resulting executable to be covered by any particular license, such as
   the GNU General Public License that covers much of the rest of prc-tools.
   However this does not invalidate any other reasons why the executable
   might be covered by such a license (such as the use of other functions
   from other header files that are so-covered).  */

#ifndef _PRC_TOOLS_STDLIB_H
#include_next <stdlib.h>

#ifdef __OPTIMIZE__

#ifdef __cplusplus
extern "C" {
#endif

/* 7.20.3  Memory management functions.  */

#include <MemoryMgr.h>

extern inline void *malloc (size_t _size) {
  return _size? MemPtrNew (_size) : NULL;
  }

extern inline void free (void *_ptr) {
  if (_ptr)  MemPtrFree (_ptr);
  }

#ifdef __cplusplus
}
#endif

#endif
#endif
