/*							exp10.c
 *
 *	Base 10 exponential function
 *      (Common antilogarithm)
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, exp10();
 *
 * y = exp10( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns 10 raised to the x power.
 *
 * Range reduction is accomplished by expressing the argument
 * as 10**x = 2**n 10**f, with |f| < 0.5 log10(2).
 * A polynomial approximates 10**f.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -38,+38     100000      9.8e-8      2.8e-8
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * exp10 underflow    x < -MAXL10        0.0
 * exp10 overflow     x > MAXL10       MAXNUM
 *
 * IEEE single arithmetic: MAXL10 = 38.230809449325611792.
 *
 */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "mconf.h"

static float P[] = {
 2.063216740311022E-001,
 5.420251702225484E-001,
 1.171292686296281E+000,
 2.034649854009453E+000,
 2.650948748208892E+000,
 2.302585167056758E+000
};

/*static float LOG102 = 3.01029995663981195214e-1;*/
static float LOG210 = 3.32192809488736234787e0;
static float LG102A = 3.00781250000000000000E-1;
static float LG102B = 2.48745663981195213739E-4;
static float MAXL10 = 38.230809449325611792;




extern float MAXNUMF;

#ifdef ANSIC
float floor(float), ldexp(float, int), polevl(float, float *, int);

float exp10(float xx)
#else
float floor(), ldexp(), polevl();

float exp10(xx)
double xx;
#endif
{
float x, px, qx;
short n;

x = xx;
if( x > MAXL10 )
	{
	mtherr( "exp10", OVERFLOW );
	return( MAXNUMF );
	}

if( x < -MAXL10 )	/* Would like to use MINLOG but can't */
	{
	mtherr( "exp10", UNDERFLOW );
	return(0.0f);
	}

/* The following is necessary because range reduction blows up: */
if( x == 0 )
	return(1.0f);

/* Express 10**x = 10**g 2**n
 *   = 10**g 10**( n log10(2) )
 *   = 10**( g + n log10(2) )
 */
px = x * LOG210;
qx = floor( px + 0.5f );
n = qx;
x -= qx * LG102A;
x -= qx * LG102B;

/* rational approximation for exponential
 * of the fractional part:
 * 10**x - 1  =  2x P(x**2)/( Q(x**2) - P(x**2) )
 */
px = 1.0f + x * polevl( x, P, 5 );

/* multiply by power of 2 */
x = ldexp( px, n );

return(x);
}
