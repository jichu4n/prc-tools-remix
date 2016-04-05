/* stdint.h: ISO/IEC 9899:1999  7.18  Integer types.

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

#ifndef _PRC_TOOLS_STDINT_H
#define _PRC_TOOLS_STDINT_H

/* 7.18.1  Integer types.  */

typedef   signed char		 int8_t,   int_least8_t,   int_fast8_t;
typedef unsigned char		uint8_t,  uint_least8_t,  uint_fast8_t;
typedef   signed short int	 int16_t,  int_least16_t;
typedef unsigned short int	uint16_t, uint_least16_t;
typedef   signed int					   int_fast16_t;
typedef unsigned int					  uint_fast16_t;
typedef   signed long int	 int32_t,  int_least32_t,  int_fast32_t;
typedef unsigned long int	uint32_t, uint_least32_t, uint_fast32_t;
typedef   signed long long int	 int64_t,  int_least64_t,  int_fast64_t;
typedef unsigned long long int	uint64_t, uint_least64_t, uint_fast64_t;

typedef   signed long int	 intptr_t;
typedef unsigned long int	uintptr_t;

typedef   signed long long int	 intmax_t;
typedef unsigned long long int	uintmax_t;

/* 7.18.2  Limits of specified-width integer types.  */

#if !defined __cplusplus || defined __STDC_LIMIT_MACROS

#define  INT8_MIN		(-128)
#define  INT8_MAX		(+127)
#define UINT8_MAX		(+255)

#define  INT16_MIN		(-32767 - 1)
#define  INT16_MAX		(+32767)
#define UINT16_MAX		(+65535U)

#define  INT32_MIN		(-2147483647L - 1)
#define  INT32_MAX		(+2147483647L)
#define UINT32_MAX		(+4294967295UL)

#define  INT64_MIN		(-9223372036854775807LL - 1)
#define  INT64_MAX		(+9223372036854775807LL)
#define UINT64_MAX		(+18446744073709551615ULL)

#define  INT_LEAST8_MIN		 INT8_MIN
#define  INT_LEAST8_MAX		 INT8_MAX
#define UINT_LEAST8_MAX		UINT8_MAX

#define  INT_LEAST16_MIN	 INT16_MIN
#define  INT_LEAST16_MAX	 INT16_MAX
#define UINT_LEAST16_MAX	UINT16_MAX

#define  INT_LEAST32_MIN	 INT32_MIN
#define  INT_LEAST32_MAX	 INT32_MAX
#define UINT_LEAST32_MAX	UINT32_MAX

#define  INT_LEAST64_MIN	 INT64_MIN
#define  INT_LEAST64_MAX	 INT64_MAX
#define UINT_LEAST64_MAX	UINT64_MAX

#define  INT_FAST8_MIN		 INT8_MIN
#define  INT_FAST8_MAX		 INT8_MAX
#define UINT_FAST8_MAX		UINT8_MAX

#if __INT_MAX__ == 32767
#define  INT_FAST16_MIN		 INT16_MIN
#define  INT_FAST16_MAX		 INT16_MAX
#define UINT_FAST16_MAX		UINT16_MAX
#else
#define  INT_FAST16_MIN		 INT32_MIN
#define  INT_FAST16_MAX		 INT32_MAX
#define UINT_FAST16_MAX		UINT32_MAX
#endif

#define  INT_FAST32_MIN		 INT32_MIN
#define  INT_FAST32_MAX		 INT32_MAX
#define UINT_FAST32_MAX		UINT32_MAX

#define  INT_FAST64_MIN		 INT64_MIN
#define  INT_FAST64_MAX		 INT64_MAX
#define UINT_FAST64_MAX		UINT64_MAX

#define  INTPTR_MIN		 INT32_MIN
#define  INTPTR_MAX		 INT32_MAX
#define UINTPTR_MAX		UINT32_MAX

#define  INTMAX_MIN		 INT64_MIN
#define  INTMAX_MAX		 INT64_MAX
#define UINTMAX_MAX		UINT64_MAX

/* 7.18.3  Limits of other integer types [no sig_atomic_t, wchar_t, wint_t].  */

#define PTRDIFF_MIN		 INT32_MIN
#define PTRDIFF_MAX		 INT32_MAX

#define SIZE_MAX		UINT32_MAX

#endif

/* 7.18.4  Macros for integer constants.  */

#if !defined __cplusplus || defined __STDC_CONSTANT_MACROS

#define  INT8_C(n)	n
#define UINT8_C(n)	n ## U
#define  INT16_C(n)	n
#define UINT16_C(n)	n ## U
#define  INT32_C(n)	n ##  L
#define UINT32_C(n)	n ## UL
#define  INT64_C(n)	n ##  LL
#define UINT64_C(n)	n ## ULL

#define  INTMAX_C(n)	n ##  LL
#define UINTMAX_C(n)	n ## ULL

#endif

#endif
