/* C support library memory management functions for Palm OS.

   Author: John Marshall  (They're not very complicated :-))

   This code is in the public domain.  */

#ifdef BOOTSTRAP
#include "bootstrap.h"
#else
#include <MemoryMgr.h>
#include "NewTypes.h"
#endif

#include "stdlib.h"

#ifdef Lmalloc

void *
malloc (size_t size) {
  return MemPtrNew (size);
  }

#endif
#ifdef Lfree

void
free (void *ptr) {
  if (ptr)  MemPtrFree (ptr);
  }

#endif
#ifdef Lcalloc

void *
calloc (size_t nmemb, size_t memb_size) {
  UInt32 size = nmemb * memb_size;
  void *ptr = MemPtrNew (size);
  if (ptr)
    MemSet (ptr, size, 0);
  return ptr;
  }

#endif
#ifdef Lrealloc

void *
realloc (void *ptr, size_t size) {
  if (ptr == NULL)
    return MemPtrNew (size);
  else if (size == 0) {
    if (ptr)  MemPtrFree (ptr);
    return NULL;
    }
  else if (MemPtrResize (ptr, size) == 0)
    return ptr;
  else {
    void *newptr = MemPtrNew (size);
    if (newptr) {
      /* We must copy min(oldsize,newsize) bytes; since MemPtrResize always
	 succedes when shrinking, the old size is always the minimum.  */
      MemMove (newptr, ptr, MemPtrSize (ptr));
      MemPtrFree (ptr);
      }
    return newptr;
    }
  }

#endif
