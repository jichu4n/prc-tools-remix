/* NewTypes.h: Detect the SDK in use, and ensure new-style types are defined.

   Placed in the public domain by John Marshall.

   This file is part of prc-tools, but is not licensed in the same way as the
   majority of prc-tools.  The contents of this file are in the public domain.

   One particular consequence of this is that you may use this header file
   in Palm OS executables without concern that doing so by itself causes
   the resulting executable to be covered by any particular license, such as
   the GNU General Public License that covers much of the rest of prc-tools.
   However this does not invalidate any other reasons why the executable
   might be covered by such a license (such as the use of functions from
   other header files that are so-covered).  */

#ifndef _PRC_TOOLS_NEWTYPES_H
#define _PRC_TOOLS_NEWTYPES_H

#ifndef SDK_VERSION

/* #defines corresponding to new functionality in each SDK were added
   to BuildRules.h (in pre-3.5 SDKs) or BuildDefines.h (3.5 and later).
   We can use those #defines to identify each SDK.  */

#if !defined __BUILDRULES_H__ && !defined __BUILDDEFINES_H__
/* But we can't just #include BuildRules.h or BuildDefines.h itself because
   we don't know which of the two exists.  This header exists in all SDKs
   and pulls in each desired file without adding too much else.  */
#include <M68KHwr.h>
#endif

/* 5.0 R2 did introduce a CPU_ARM macro, but more uniquely it also added
   some screen density constants.  */
#if defined SCREEN_DENSITY_STANDARD
#define SDK_VERSION  52

/* 5.0 should have introduced an ARM macro for use with CPU_TYPE, but the
   only change actually made was the introduction of a version macro.  */
#elif defined PALMOS_SDK_VERSION
#define SDK_VERSION  50

/* 4.0 and its Update 1 introduced tracing, but there were no reliable
   differences between their BuildDefaults.h or BuildDefines.h (the only
   actual difference was in DO_NOT_ALLOW_ACCESS_TO_INTERNALS_OF_STRUCTS,
   but developers were encouraged to edit that themselves), so we have to
   look further afield to distinguish between the two.  */
#elif defined TRACE_OUTPUT_ON

#include <Chars.h>
#ifndef vchrHardEarbud
#define SDK_VERSION  41
#else
#define SDK_VERSION  40
#endif

/* 3.5 hinted at the conceivability of a Simulator running on Unix.  */
#elif defined EMULATION_UNIX
#define SDK_VERSION  35

/* 3.1 added Japanese support.  */
#elif defined LANGUAGE_JAPANESE
#define SDK_VERSION  31

/* 3.0 introduced the v2 memory manager.  */
#elif defined MEMORY_VERSION_2
#define SDK_VERSION  30

/* 2.0 added Italian support.  */
#elif defined LANGUAGE_ITALIAN
#define SDK_VERSION  20

#else
#define SDK_VERSION  10
#endif

#endif


#if SDK_VERSION < 35
#include <Common.h>  /* Ensure that the old names have been defined.  */

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
