extern void __do_bhook(Word, Ptr, Word);
extern void __do_ehook(Word, Ptr, Word);
extern void __do_ctors(void);
extern void __do_dtors(void);

extern void _GccRelocateData (void);
extern void _RelocateChain (Int offset, void *base);

extern int _GdbStartDebug (Word);

extern ULong start ();
