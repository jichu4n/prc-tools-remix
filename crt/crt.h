extern void __do_bhook (UInt16, void *, UInt16);
extern void __do_ehook (UInt16, void *, UInt16);
extern void __do_ctors (void);
extern void __do_dtors (void);

extern void _GccRelocateData (void);
extern void _RelocateChain (Int16 offset, void *base);

extern int _GdbStartDebug (UInt16);

extern UInt32 start ();

#define UNUSED_PARAM  __attribute__ ((unused))
