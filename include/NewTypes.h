#ifndef NEWTYPES_H
#define NEWTYPES_H

#ifndef SDK_VERSION

/* 3.5 uses <PalmTypes.h> instead of <Common.h>.  */
#if defined(__PALMTYPES_H__)
#define SDK_VERSION  35

/* Otherwise, all versions of Common.h unconditionally #include
   <BuildRules.h>.  #defines corresponding to new functionality in
   each SDK were added to this file.  */

/* 3.1 added Japanese support.  */
#elif defined(LANGUAGE_JAPANESE)
#define SDK_VERSION  31

/* 3.0 introduced the v2 memory manager.  */
#elif defined(MEMORY_VERSION_2)
#define SDK_VERSION  30

/* 2.0 added Italian support.  */
#elif defined(LANGUAGE_ITALIAN)
#define SDK_VERSION  20

#else
#define SDK_VERSION  10
#endif

#endif


#if SDK_VERSION < 35
typedef Byte UInt8;
typedef SByte Int8;

/* The 3.0 and 3.1 SDKs already typedef the 16 and 32 bit types.  */
#if SDK_VERSION < 30
typedef Word UInt16;
typedef SWord Int16;
typedef DWord UInt32;
typedef SDWord Int32;
#endif

typedef VoidPtr MemPtr;
typedef VoidHand MemHandle;
#endif

#endif
