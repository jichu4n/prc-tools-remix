#ifndef NEWTYPES_H
#define NEWTYPES_H

#ifndef SDK_VERSION

/* Start by assuming that the minimum possible number of headers have been
   #included.  On a pre-3.5 SDK, that means that <Common.h> is one of them.
   Similarly, in a 3.5 or later SDK, <PalmTypes.h> must have been #included.

   All versions of <Common.h> unconditionally #include <BuildRules.h>.
   All versions of <PalmTypes.h> unconditionally #include <BuildDefaults.h>,
   and thence <BuildDefines.h>.

   #defines corresponding to new functionality in each SDK were added to
   <BuildRules.h> or <BuildDefines.h>.  We can use those #defines to
   identify the SDK.  */

/* 5.0 should have introduced an ARM macro for use with CPU_TYPE, but the
   only change actually made was the introduction of a version macro.  */
#if defined(PALMOS_SDK_VERSION)
#define SDK_VERSION  50

/* 4.0 introduced tracing.  */
#elif defined(TRACE_OUTPUT_ON)
#define SDK_VERSION  40

/* 3.5 uses <PalmTypes.h> instead of <Common.h>.  */
#elif defined(__PALMTYPES_H__)
#define SDK_VERSION  35

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
