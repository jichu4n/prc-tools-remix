extern void __do_bhook (UInt16, void *, UInt16);
extern void __do_ehook (UInt16, void *, UInt16);
extern void __do_ctors (void);
extern void __do_dtors (void);

extern void _GccRelocateData (void);
extern void _RelocateChain (Int16 offset, void *base);

extern char data_start;
extern char bss_start;

#define UNUSED_PARAM  __attribute__ ((unused))
