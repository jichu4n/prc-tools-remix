#ifndef __GLIB_H__
#define __GLIB_H__

struct LibRef {
    void *jmptable;
    void *globals;
    DmOpenRef *dbref;
/* Entries above here are referenced in scrt0.o; if you change them,
   incompatibilities will result. */
    void (*cleanfcn)(UInt16,struct LibRef *);
    MemHandle GLibHand;
    UInt32 creator;
    UInt16 refcount;
};

struct LibRef *GLibOpen(UInt32, Char*);
void GLibClose(struct LibRef *);

#endif
