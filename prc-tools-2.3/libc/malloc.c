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
  /* MemPtrNew(0) returns NULL (quietly) on all the Palm OS versions tested,
     but there is evidence that this may not be the case on *all* versions.
     It seems wise not to depend on it in the future either.  */
  void *p = NULL;
  if (size)  p = MemPtrNew (size);
  return p;
  }

#endif
#ifdef Lfree

void
free (void *ptr) {
  /* MemPtrFree(NULL) aborts with a fatal alert, so we need to check.  */
  if (ptr)  MemPtrFree (ptr);
  }

#endif
#ifdef Lcalloc

void *
calloc (size_t nmemb, size_t memb_size) {
  size_t size = nmemb * memb_size;
  void *ptr = malloc (size);
  if (ptr)
    MemSet (ptr, size, 0);
  return ptr;
  }

#endif
#ifdef Lrealloc

void *
realloc (void *ptr, size_t size) {
  if (ptr == NULL)
    return malloc (size);
  else if (size == 0) {
    MemPtrFree (ptr);  /* At this point, we know that PTR is non-null.  */
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
