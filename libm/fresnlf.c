/*							fresnl.c
 *
 *	Fresnel integral
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, S, C;
 * void fresnl();
 *
 * fresnl( x, _&S, _&C );
 *
 *
 * DESCRIPTION:
 *
 * Evaluates the Fresnel integrals
 *
 *           x
 *           -
 *          | |
 * C(x) =   |   cos(pi/2 t**2) dt,
 *        | |
 *         -
 *          0
 *
 *           x
 *           -
 *          | |
 * S(x) =   |   sin(pi/2 t**2) dt.
 *        | |
 *         -
 *          0
 *
 *
 * The integrals are evaluated by power series for small x.
 * For x >= 1 auxiliary functions f(x) and g(x) are employed
 * such that
 *
 * C(x) = 0.5 + f(x) sin( pi/2 x**2 ) - g(x) cos( pi/2 x**2 )
 * S(x) = 0.5 - f(x) cos( pi/2 x**2 ) - g(x) sin( pi/2 x**2 )
 *
 *
 *
 * ACCURACY:
 *
 *  Relative error.
 *
 * Arithmetic  function   domain     # trials      peak         rms
 *   IEEE       S(x)      0, 10       30000       1.1e-6      1.9e-7
 *   IEEE       C(x)      0, 10       30000       1.1e-6      2.0e-7
 */

/*
Cephes Math Library Release 2.1:  January, 1989
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

/* S(x) for small x */
static float sn[7] = {
 1.647629463788700E-009,
-1.522754752581096E-007,
 8.424748808502400E-006,
-3.120693124703272E-004,
 7.244727626597022E-003,
-9.228055941124598E-002,
 5.235987735681432E-001
};

/* C(x) for small x */
static float cn[7] = {
 1.416802502367354E-008,
-1.157231412229871E-006,
 5.387223446683264E-005,
-1.604381798862293E-003,
 2.818489036795073E-002,
-2.467398198317899E-001,
 9.999999760004487E-001
};


/* Auxiliary function f(x) */
static float fn[8] = {
-1.903009855649792E+012,
 1.355942388050252E+011,
-4.158143148511033E+009,
 7.343848463587323E+007,
-8.732356681548485E+005,
 8.560515466275470E+003,
-1.032877601091159E+002,
 2.999401847870011E+000
};

/* Auxiliary function g(x) */
static float gn[8] = {
-1.860843997624650E+011,
 1.278350673393208E+010,
-3.779387713202229E+008,
 6.492611570598858E+006,
-7.787789623358162E+004,
 8.602931494734327E+002,
-1.493439396592284E+001,
 9.999841934744914E-001
};


extern float PIF, PIO2F;

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float polevl( float, float *, int );
float cos(float), sin(float);
#else
float polevl(), cos(), sin();
#endif

#ifdef ANSIC
void fresnl( float xxa, float *ssa, float *cca )
#else
int fresnl( xxa, ssa, cca )
double xxa;
float *ssa, *cca;
#endif
{
float f, g, cc, ss, c, s, t, u, x, x2;
/*debug double t1;*/

x = xxa;
x = fabsf(x);
x2 = x * x;
if( x2 < 2.5625f )
	{
	t = x2 * x2;
	ss = x * x2 * polevl( t, sn, 6);
	cc = x * polevl( t, cn, 6);
	goto done;
	}

if( x > 36974.0f )
	{
	cc = 0.5f;
	ss = 0.5f;
	goto done;
	}


/*		Asymptotic power series auxiliary functions
 *		for large argument
 */
	x2 = x * x;
	t = PIF * x2;
	u = 1.0f/(t * t);
	t = 1.0f/t;
	f = 1.0f - u * polevl( u, fn, 7);
	g = t * polevl( u, gn, 7);

	t = PIO2F * x2;
	c = cos(t);
	s = sin(t);
	t = PIF * x;
	cc = 0.5f  +  (f * s  -  g * c)/t;
	ss = 0.5f  -  (f * c  +  g * s)/t;

done:
if( xxa < 0.0f )
	{
	cc = -cc;
	ss = -ss;
	}

*cca = cc;
*ssa = ss;
#if !ANSIC
return 0;
#endif
}
