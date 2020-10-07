/*						rgamma.c
 *
 *	Reciprocal gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, rgamma();
 *
 * y = rgamma( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns one divided by the gamma function of the argument.
 *
 * The function is approximated by a Chebyshev expansion in
 * the interval [0,1].  Range reduction is by recurrence
 * for arguments between -34.034 and +34.84425627277176174.
 * 1/MAXNUMF is returned for positive arguments outside this
 * range.
 *
 * The reciprocal gamma function has no singularities,
 * but overflow and underflow may occur for large arguments.
 * These conditions return either MAXNUMF or 1/MAXNUMF with
 * appropriate sign.
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE     -34,+34      100000      8.9e-7      1.1e-7
 */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1985, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

/* Chebyshev coefficients for reciprocal gamma function
 * in interval 0 to 1.  Function is 1/(x gamma(x)) - 1
 */

static float R[] = {
 1.08965386454418662084E-9,
-3.33964630686836942556E-8,
 2.68975996440595483619E-7,
 2.96001177518801696639E-6,
-8.04814124978471142852E-5,
 4.16609138709688864714E-4,
 5.06579864028608725080E-3,
-6.41925436109158228810E-2,
-4.98558728684003594785E-3,
 1.27546015610523951063E-1
};


/* static char name[] = "rgamma"; */

extern float PIF, MAXLOGF, MAXNUMF;



#ifdef ANSIC
float chbevl(float, float *, int);
float exp(float), log(float), sin(float), lgam(float);

float rgamma(float xx)
#else
float chbevl(), exp(), log(), sin(), lgam();

float rgamma(xx)
double xx;
#endif
{
float x, w, y, z;
int sign;

x = xx;
if( x > 34.84425627277176174f)
	{
	mtherr( name, UNDERFLOW );
	return(1.0f/MAXNUMF);
	}
if( x < -34.034f )
	{
	w = -x;
	z = sin( PIF*w );
	if( z == 0.0f )
		return(0.0f);
	if( z < 0.0f )
		{
		sign = 1;
		z = -z;
		}
	else
		sign = -1;

	y = log( w * z / PIF ) + lgam(w);
	if( y < -MAXLOGF )
		{
		mtherr( name, UNDERFLOW );
		return( sign * 1.0f / MAXNUMF );
		}
	if( y > MAXLOGF )
		{
		mtherr( name, OVERFLOW );
		return( sign * MAXNUMF );
		}
	return( sign * exp(y));
	}
z = 1.0f;
w = x;

while( w > 1.0f )	/* Downward recurrence */
	{
	w -= 1.0f;
	z *= w;
	}
while( w < 0.0f )	/* Upward recurrence */
	{
	z /= w;
	w += 1.0f;
	}
if( w == 0.0f )		/* Nonpositive integer */
	return(0.0f);
if( w == 1.0f )		/* Other integer */
	return( 1.0f/z );

y = w * ( 1.0f + chbevl( 4.0f*w-2.0f, R, 10 ) ) / z;
return(y);
}
