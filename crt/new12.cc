/* These are horribly corrupted no handler using, no throwing, versions
   of the support functions in gcc/cp/new[12].cc.  Use at your own risk.
   I don't really know what I'm doing here.  */

#include "new"

/* FIXME: tidy up the headers enough that we can use #include <stdlib.h> */
extern "C" void *malloc (size_t);
extern "C" void free (void *);

#ifdef Lnew

void *
operator new (size_t sz) throw (std::bad_alloc)
{
  return malloc (sz? sz : 1);
}

#endif
#ifdef Lvnew

void *
operator new[] (size_t sz) throw (std::bad_alloc)
{
  return malloc (sz? sz : 1);
}

#endif
#ifdef Lnewnt

void *
operator new (size_t sz, const std::nothrow_t&) throw ()
{
  return malloc (sz? sz : 1);
}

#endif
#ifdef Lvnewnt

void *
operator new[] (size_t sz, const std::nothrow_t&) throw ()
{
  return malloc (sz? sz : 1);
}

#endif
#ifdef Ldel

void
operator delete (void *ptr) throw ()
{
  free (ptr);
}

#endif
#ifdef Lvdel

void
operator delete[] (void *ptr) throw ()
{
  free (ptr);
}

#endif
#ifdef Ldelnt

void
operator delete (void *ptr, const std::nothrow_t&) throw ()
{
  free (ptr);
}

#endif
#ifdef Lvdelnt

void
operator delete[] (void *ptr, const std::nothrow_t&) throw ()
{
  free (ptr);
}

#endif
