/* nfm.c: GCC entry points to the New Float Manager.

   This code was written by Palm Computing, Inc., and is in the public
   domain.  In particular, object code compiled from this code may be
   freely linked into your programs.  */

#include <ErrorMgr.h>
#include "NewTypes.h"

#if SDK_VERSION >= 35

#include <FloatMgr.h>
#define TRAP(sel)  FLOAT_EM_TRAP(sysFloat##sel)

#else

/* Earlier SDKs don't enable GCC to call selectorized traps such as the New
   Float Manager traps, so we define the relevant machinery ourselves.  */

#define Str(X)  #X

#define TRAP(sel) \
  __attribute__ ((callseq ("moveq #" Str(sel) ",%%d2; trap #15; dc.w 0xA306")))

#define Em_f_itof	 4
#define Em_f_lltof	 6
#define Em_d_itod	 8
#define Em_d_lltod	10
#define Em_f_ftod	11
#define Em_d_dtof	12
#define Em_f_ftou	17
#define Em_f_ftoi	18
#define Em_f_ftoull	19
#define Em_f_ftoll	20
#define Em_d_dtou	21
#define Em_d_dtoi	22
#define Em_d_dtoull	23
#define Em_d_dtoll	24
#define Em_f_neg	45
#define Em_f_add	46
#define Em_f_mul	47
#define Em_f_sub	48
#define Em_f_div	49
#define Em_d_neg	50
#define Em_d_add	51
#define Em_d_mul	52
#define Em_d_sub	53
#define Em_d_div	54

#endif


typedef int SItype __attribute__ ((mode (SI)));
typedef int DItype __attribute__ ((mode (DI)));
typedef int SFtype __attribute__ ((mode (SF)));
typedef int DFtype __attribute__ ((mode (DF)));
typedef unsigned int USItype __attribute__ ((mode (SI)));
typedef unsigned int UDItype __attribute__ ((mode (DI)));


/* These functions are the same as the ones in NewFloatMgr.h, except that
   the types have been replaced by the equivalent native GCC types, so that
   we don't have to cast everything to and from NewFloatMgr.h's types.

   We can't use some of the NewFloatMgr functions directory because they
   return structures, and GCC doesn't do this in the same way as CodeWarrior.
   Fortunately, the CW ABI is such that the XXX_aux functions do look the
   same as the corresponding functions in the new float manager.  See

     news://news.massena.com/01bc20ff$0671c580$21fc6bcc@david  */


SFtype	f_itof		(SItype)		TRAP(Em_f_itof);
SFtype	f_lltof		(DItype)		TRAP(Em_f_lltof);
void	d_itod_aux	(DFtype *, SItype)	TRAP(Em_d_itod);
void	d_lltod_aux	(DFtype *, DItype)	TRAP(Em_d_lltod);
void	f_ftod_aux	(DFtype *, SFtype)	TRAP(Em_f_ftod);
SFtype	d_dtof		(DFtype)		TRAP(Em_d_dtof);
USItype	f_ftou		(SFtype)		TRAP(Em_f_ftou);
SItype	f_ftoi		(SFtype)		TRAP(Em_f_ftoi);
void	f_ftoull_aux	(UDItype *, SFtype)	TRAP(Em_f_ftoull);
void	f_ftoll_aux	(DItype *, SFtype)	TRAP(Em_f_ftoll);
USItype	d_dtou		(DFtype)		TRAP(Em_d_dtou);
SItype	d_dtoi		(DFtype)		TRAP(Em_d_dtoi);
void	d_dtoull_aux	(UDItype *, DFtype)	TRAP(Em_d_dtoull);
void	d_dtoll_aux	(DItype *, DFtype)	TRAP(Em_d_dtoll);
SFtype	f_neg		(SFtype)		TRAP(Em_f_neg);
SFtype	f_add		(SFtype, SFtype)	TRAP(Em_f_add);
SFtype	f_mul		(SFtype, SFtype)	TRAP(Em_f_mul);
SFtype	f_sub		(SFtype, SFtype)	TRAP(Em_f_sub);
SFtype	f_div		(SFtype, SFtype)	TRAP(Em_f_div);
void	d_neg_aux	(DFtype *, DFtype)	TRAP(Em_d_neg);
void	d_add_aux	(DFtype *, DFtype, DFtype) TRAP(Em_d_add);
void	d_mul_aux	(DFtype *, DFtype, DFtype) TRAP(Em_d_mul);
void	d_sub_aux	(DFtype *, DFtype, DFtype) TRAP(Em_d_sub);
void	d_div_aux	(DFtype *, DFtype, DFtype) TRAP(Em_d_div);


#ifdef L__floatsisf

SFtype
__floatsisf (SItype x) {
  return f_itof (x);
  }

#endif
#ifdef L__floatdisf

SFtype
__floatdisf (DItype x) {
  return f_lltof (x);
  }

#endif
#ifdef L__floatsidf

DFtype
__floatsidf (SItype x) {
  DFtype z;
  d_itod_aux (&z, x);
  return z;
  }

#endif
#ifdef L__floatdidf

DFtype
__floatdidf (DItype x) {
  DFtype z;
  d_lltod_aux (&z, x);
  return z;
  }

#endif
#ifdef L__extendsfdf2

DFtype
__extendsfdf2 (SFtype x) {
  DFtype z;
  f_ftod_aux (&z, x);
  return z;
  }

#endif
#ifdef L__truncdfsf2

SFtype
__truncdfsf2 (DFtype x) {
  return d_dtof (x);
  }

#endif
#ifdef L__fixunssfsi

USItype
__fixunssfsi (SFtype x) {
  return f_ftou (x);
  }

#endif
#ifdef L__fixsfsi

SItype
__fixsfsi (SFtype x) {
  return f_ftoi (x);
  }

#endif
#ifdef L__fixunssfdi

UDItype
__fixunssfdi (SFtype x) {
  UDItype z;
  f_ftoull_aux (&z, x);
  return z;
  }

#endif
#ifdef L__fixsfdi

DItype
__fixsfdi (SFtype x) {
  DItype z;
  f_ftoll_aux (&z, x);
  return z;
  }

#endif
#ifdef L__fixunsdfsi

USItype
__fixunsdfsi (DFtype x) {
  return d_dtou (x);
  }

#endif
#ifdef L__fixdfsi

SItype
__fixdfsi (DFtype x) {
  return d_dtoi (x);
  }

#endif
#ifdef L__fixunsdfdi

UDItype
__fixunsdfdi (DFtype x) {
  UDItype z;
  d_dtoull_aux (&z, x);
  return z;
  }

#endif
#ifdef L__fixdfdi

DItype
__fixdfdi (DFtype x) {
  DItype z;
  d_dtoll_aux (&z, x);
  return z;
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
  static const signed char map[] = { 0, -1, 1, 1 };
  return map[f_cmp (x, y)];
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
  return f_neg (x);
  }

#endif
#ifdef L__addsf3

SFtype
__addsf3 (SFtype x, SFtype y) {
  return f_add (x, y);
  }

#endif
#ifdef L__mulsf3

SFtype
__mulsf3 (SFtype x, SFtype y) {
  return f_mul (x, y);
  }

#endif
#ifdef L__subsf3

SFtype
__subsf3 (SFtype x, SFtype y) {
  return f_sub (x, y);
  }

#endif
#ifdef L__divsf3

SFtype
__divsf3 (SFtype x, SFtype y) {
  return f_div (x, y);
  }

#endif
#ifdef L__negdf2

DFtype
__negdf2 (DFtype x) {
  DFtype z;
  d_neg_aux (&z, x);
  return z;
  }

#endif
#ifdef L__adddf3

DFtype
__adddf3 (DFtype x, DFtype y) {
  DFtype z;
  d_add_aux (&z, x, y);
  return z;
  }

#endif
#ifdef L__muldf3

DFtype
__muldf3 (DFtype x, DFtype y) {
  DFtype z;
  d_mul_aux (&z, x, y);
  return z;
  }

#endif
#ifdef L__subdf3

DFtype
__subdf3 (DFtype x, DFtype y) {
  DFtype z;
  d_sub_aux (&z, x, y);
  return z;
  }

#endif
#ifdef L__divdf3

DFtype
__divdf3 (DFtype x, DFtype y) {
  DFtype z;
  d_div_aux (&z, x, y);
  return z;
  }

#endif
