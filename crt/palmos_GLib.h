#ifndef __GLIB_H__
#define __GLIB_H__

struct LibRef {
    void *jmptable;
    void *globals;
    DmOpenRef *dbref;
/* Entries above here are referenced in scrt0.o; if you change them,
   incompatibilities will result. */
    void (*cleanfcn)(UInt,struct LibRef *);
    VoidHand GLibHand;
    ULong creator;
    UInt refcount;
};

struct LibRef *GLibOpen(ULong, CharPtr);
void GLibClose(struct LibRef *);

#endif
