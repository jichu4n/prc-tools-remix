/* nfm.c: GCC entry points to the New Float Manager.

   Copyright (c) 2000 Palm Computing, Inc. or its subsidiaries.
   All rights reserved.  */

#include <ErrorMgr.h>
#include "NewTypes.h"

#if SDK_VERSION < 35
#include <NewFloatMgr.h>
#else
/* The "New" was dropped in the 3.5 SDK.  */
#include <FloatMgr.h>
#endif


/* We can't use some of the new float manager functions directly because
   they return structures, and GCC doesn't do this the same way as
   CodeWarrior.  Fortunately, the CW ABI is such that the following
   functions do look the same as the corresponding functions in the
   new float manager.  See

     news://news.massena.com/01bc20ff$0671c580$21fc6bcc@david  */

void _f_ftoll_aux (sfpe_long_long *, FlpFloat)
  FLOAT_EM_TRAP(sysFloatEm_f_ftoll);
void _f_ftoull_aux (sfpe_unsigned_long_long *, FlpFloat)
  FLOAT_EM_TRAP(sysFloatEm_f_ftoull);
void _f_ftod_aux (FlpDouble *, FlpFloat) FLOAT_EM_TRAP(sysFloatEm_f_ftod);

void _d_dtoll_aux (sfpe_long_long *, FlpDouble)
  FLOAT_EM_TRAP(sysFloatEm_d_dtoll);
void _d_dtoull_aux (sfpe_unsigned_long_long *, FlpDouble)
  FLOAT_EM_TRAP(sysFloatEm_d_dtoull);
void _d_itod_aux (FlpDouble *, Int32) FLOAT_EM_TRAP(sysFloatEm_d_itod);
void _d_lltod_aux (FlpDouble *, sfpe_long_long)
  FLOAT_EM_TRAP(sysFloatEm_d_lltod);

void _d_neg_aux (FlpDouble *, FlpDouble) FLOAT_EM_TRAP(sysFloatEm_d_neg);

void _d_add_aux (FlpDouble *, FlpDouble, FlpDouble)
  FLOAT_EM_TRAP(sysFloatEm_d_add);
void _d_mul_aux (FlpDouble *, FlpDouble, FlpDouble)
  FLOAT_EM_TRAP(sysFloatEm_d_mul);
void _d_sub_aux (FlpDouble *, FlpDouble, FlpDouble)
  FLOAT_EM_TRAP(sysFloatEm_d_sub);
void _d_div_aux (FlpDouble *, FlpDouble, FlpDouble)
  FLOAT_EM_TRAP(sysFloatEm_d_div);


typedef int SItype __attribute__ ((mode (SI)));
typedef int DItype __attribute__ ((mode (DI)));
typedef int SFtype __attribute__ ((mode (SF)));
typedef int DFtype __attribute__ ((mode (DF)));
typedef unsigned int USItype __attribute__ ((mode (SI)));
typedef unsigned int UDItype __attribute__ ((mode (DI)));


#define CVT_FUNCTION(NAME, FROM, TO)					\
  static inline TO NAME (FROM x) {					\
    union { FROM a; TO b; } u;						\
    u.a = x;								\
    return u.b;								\
    }

#define CVT_FUNCTIONS(GCCNAME, NFMNAME)					\
  CVT_FUNCTION (NFMNAME##_of, GCCNAME, NFMNAME)				\
  CVT_FUNCTION (of_##NFMNAME, NFMNAME, GCCNAME)

CVT_FUNCTIONS (DItype, sfpe_long_long)
CVT_FUNCTIONS (UDItype, sfpe_unsigned_long_long)
CVT_FUNCTIONS (SFtype, FlpFloat)
CVT_FUNCTIONS (DFtype, FlpDouble)


#ifdef L__floatsisf

SFtype
__floatsisf (SItype x) {
  return of_FlpFloat (_f_itof (x));
  }

#endif
#ifdef L__floatdisf

SFtype
__floatdisf (DItype x) {
  return of_FlpFloat (_f_lltof (sfpe_long_long_of (x)));
  }

#endif
#ifdef L__floatsidf

DFtype
__floatsidf (SItype x) {
  FlpDouble z;
  _d_itod_aux (&z, x);
  return of_FlpDouble (z);
  }

#endif
#ifdef L__floatdidf

DFtype
__floatdidf (DItype x) {
  FlpDouble z;
  _d_lltod_aux (&z, sfpe_long_long_of (x));
  return of_FlpDouble (z);
  }

#endif
#ifdef L__extendsfdf2

DFtype
__extendsfdf2 (SFtype x) {
  FlpDouble z;
  _f_ftod_aux (&z, FlpFloat_of (x));
  return of_FlpDouble (z);
  }

#endif
#ifdef L__truncdfsf2

SFtype
__truncdfsf2 (DFtype x) {
  return of_FlpFloat (_d_dtof (FlpDouble_of (x)));
  }

#endif
#ifdef L__fixunssfsi

USItype
__fixunssfsi (SFtype x) {
  return _f_ftou (FlpFloat_of (x));
  }

#endif
#ifdef L__fixsfsi

SItype
__fixsfsi (SFtype x) {
  return _f_ftoi (FlpFloat_of (x));
  }

#endif
#ifdef L__fixunssfdi

UDItype
__fixunssfdi (SFtype x) {
  sfpe_unsigned_long_long z;
  _f_ftoull_aux (&z, FlpFloat_of (x));
  return of_sfpe_unsigned_long_long (z);
  }

#endif
#ifdef L__fixsfdi

DItype
__fixsfdi (SFtype x) {
  sfpe_long_long z;
  _f_ftoll_aux (&z, FlpFloat_of (x));
  return of_sfpe_long_long (z);
  }

#endif
#ifdef L__fixunsdfsi

USItype
__fixunsdfsi (DFtype x) {
  return _d_dtou (FlpDouble_of (x));
  }

#endif
#ifdef L__fixdfsi

SItype
__fixdfsi (DFtype x) {
  return _d_dtoi (FlpDouble_of (x));
  }

#endif
#ifdef L__fixunsdfdi

UDItype
__fixunsdfdi (DFtype x) {
  sfpe_unsigned_long_long z;
  _d_dtoull_aux (&z, FlpDouble_of (x));
  return of_sfpe_unsigned_long_long (z);
  }

#endif
#ifdef L__fixdfdi

DItype
__fixdfdi (DFtype x) {
  sfpe_long_long z;
  _d_dtoll_aux (&z, FlpDouble_of (x));
  return of_sfpe_long_long (z);
  }

#endif
#ifdef L__cmp_sf

asm("
.globl __cmp_sf
.globl __eqsf2
.globl __nesf2
.globl __gesf2
.globl __gtsf2
.globl __lesf2
.globl __ltsf2
__cmp_sf:
__eqsf2:
__nesf2:
__gesf2:
__gtsf2:
__lesf2:
__ltsf2:
	move.l	8(%sp),-(%sp)
	move.l	8(%sp),-(%sp)
	moveq	#25,%d2; trap #15; dc.w 0xA306  /* _f_cmp */
	addq.l	#8,%sp
	bra.w	__cmpmap
");

#if 0
/* This can be done in C about as efficiently.  But in asm, we get to map
   all the names to the same function.  :-)  */
SItype
__cmpsf2 (SFtype x, SFtype y) {
  static const short map[] = { 0, -1, 1, 1 };
  return map[_f_cmp (FlpFloat_of (x), FlpFloat_of (y))];
  }
#endif

#endif
#ifdef L__cmp_df

asm("
.globl __cmp_df
.globl __eqdf2
.globl __nedf2
.globl __gedf2
.globl __gtdf2
.globl __ledf2
.globl __ltdf2
__cmp_df:
__eqdf2:
__nedf2:
__gedf2:
__gtdf2:
__ledf2:
__ltdf2:
	move.l	16(%sp),-(%sp)
	move.l	16(%sp),-(%sp)
	move.l	16(%sp),-(%sp)
	move.l	16(%sp),-(%sp)
	moveq	#35,%d2; trap #15; dc.w 0xA306  /* _d_cmp */
	lea	16(%sp),%sp
	bra.w	__cmpmap
");

#endif
#ifdef L__cmpmap

asm("
.global __cmpmap
__cmpmap:
	move.b	(1f,%pc,%d0),%d0
	ext.w	%d0
	ext.l	%d0
	rts
1:	dc.b	0, -1, 1, 1
");

#endif
#ifdef L__negsf2

SFtype
__negsf2 (SFtype x) {
  return of_FlpFloat (_f_neg (FlpFloat_of (x)));
  }

#endif
#ifdef L__addsf3

SFtype
__addsf3 (SFtype x, SFtype y) {
  return of_FlpFloat (_f_add (FlpFloat_of (x), FlpFloat_of (y)));
  }

#endif
#ifdef L__mulsf3

SFtype
__mulsf3 (SFtype x, SFtype y) {
  return of_FlpFloat (_f_mul (FlpFloat_of (x), FlpFloat_of (y)));
  }

#endif
#ifdef L__subsf3

SFtype
__subsf3 (SFtype x, SFtype y) {
  return of_FlpFloat (_f_sub (FlpFloat_of (x), FlpFloat_of (y)));
  }

#endif
#ifdef L__divsf3

SFtype
__divsf3 (SFtype x, SFtype y) {
  return of_FlpFloat (_f_div (FlpFloat_of (x), FlpFloat_of (y)));
  }

#endif
#ifdef L__negdf2

DFtype
__negdf2 (DFtype x) {
  FlpDouble z;
  _d_neg_aux (&z, FlpDouble_of (x));
  return of_FlpDouble (z);
  }

#endif
#ifdef L__adddf3

DFtype
__adddf3 (DFtype x, DFtype y) {
  FlpDouble z;
  _d_add_aux (&z, FlpDouble_of (x), FlpDouble_of (y));
  return of_FlpDouble (z);
  }

#endif
#ifdef L__muldf3

DFtype
__muldf3 (DFtype x, DFtype y) {
  FlpDouble z;
  _d_mul_aux (&z, FlpDouble_of (x), FlpDouble_of (y));
  return of_FlpDouble (z);
  }

#endif
#ifdef L__subdf3

DFtype
__subdf3 (DFtype x, DFtype y) {
  FlpDouble z;
  _d_sub_aux (&z, FlpDouble_of (x), FlpDouble_of (y));
  return of_FlpDouble (z);
  }

#endif
#ifdef L__divdf3

DFtype
__divdf3 (DFtype x, DFtype y) {
  FlpDouble z;
  _d_div_aux (&z, FlpDouble_of (x), FlpDouble_of (y));
  return of_FlpDouble (z);
  }

#endif
