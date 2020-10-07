/*							exp2.c
 *
 *	Base 2 exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, exp2();
 *
 * y = exp2( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns 2 raised to the x power.
 *
 * Range reduction is accomplished by separating the argument
 * into an integer k and fraction f such that
 *     x    k  f
 *    2  = 2  2.
 *
 * A polynomial approximates 2**x in the basic range [-0.5, 0.5].
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE     -127,+127    100000      1.7e-7      2.8e-8
 *
 *
 * See exp.c for comments on error amplification.
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * exp underflow    x < -MAXL2        0.0
 * exp overflow     x > MAXL2         MAXNUMF
 *
 * For IEEE arithmetic, MAXL2 = 127.
 */


/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/



#include "mconf.h"
static char fname[] = {"exp2"};

static float P[] = {
 1.535336188319500E-004,
 1.339887440266574E-003,
 9.618437357674640E-003,
 5.550332471162809E-002,
 2.402264791363012E-001,
 6.931472028550421E-001
};
#define MAXL2 127.0f
#define MINL2 -127.0f



extern float MAXNUMF;

#ifdef ANSIC
float polevl(float, float *, int), floor(float), ldexp(float, int);

float exp2( float xx )
#else
float polevl(), floor(), ldexp();

float exp2(xx)
double xx;
#endif
{
float x, px;
int i0;

x = xx;
if( x > MAXL2)
	{
	mtherr( fname, OVERFLOW );
	return( MAXNUMF );
	}

if( x < MINL2 )
	{
	mtherr( fname, UNDERFLOW );
	return(0.0f);
	}

/* The following is necessary because range reduction blows up: */
if( x == 0 )
	return(1.0f);

/* separate into integer and fractional parts */
px = floor(x);
i0 = px;
x = x - px;

if( x > 0.5f )
	{
	i0 += 1;
	x -= 1.0f;
	}

/* rational approximation
 * exp2(x) = 1.0 +  xP(x)
 */
px = 1.0f + x * polevl( x, P, 5 );

/* scale by power of 2 */
px = ldexp( px, i0 );
return(px);
}
