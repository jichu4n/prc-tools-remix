/*							log10.c
 *
 *	Common logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, log10();
 *
 * y = log10( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns logarithm to the base 10 of x.
 *
 * The argument is separated into its exponent and fractional
 * parts.  The logarithm of the fraction is approximated by
 *
 *     log(1+x) = x - 0.5 x**2 + x**3 P(x).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0.5, 2.0    100000      1.3e-7      3.4e-8
 *    IEEE      0, MAXNUMF  100000      1.3e-7      2.6e-8
 *
 * In the tests over the interval [0, MAXNUM], the logarithms
 * of the random arguments were uniformly distributed over
 * [-MAXL10, MAXL10].
 *
 * ERROR MESSAGES:
 *
 * log10 singularity:  x = 0; returns -MAXL10
 * log10 domain:       x < 0; returns -MAXL10
 * MAXL10 = 38.230809449325611792
 */

/*
Cephes Math Library Release 2.1:  December, 1988
Copyright 1984, 1987, 1988 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"
/* static char fname[] = {"log10"}; */

/* Coefficients for log(1+x) = x - x**2/2 + x**3 P(x)/Q(x)
 * 1/sqrt(2) <= x < sqrt(2)
 */
static float P[] = {
 7.0376836292E-2,
-1.1514610310E-1,
 1.1676998740E-1,
-1.2420140846E-1,
 1.4249322787E-1,
-1.6668057665E-1,
 2.0000714765E-1,
-2.4999993993E-1,
 3.3333331174E-1
};


#define SQRTH 0.70710678118654752440f
#define L102A 3.0078125E-1f
#define L102B 2.48745663981195213739E-4f
#define L10EA 4.3359375E-1f
#define L10EB 7.00731903251827651129E-4f

static float MAXL10 = 38.230809449325611792f;

#ifdef ANSIC
float frexp(float, int *), polevl(float, float *, int);

float log10(float xx)
#else
float frexp(), polevl();

float log10(xx)
double xx;
#endif
{
float x, y, z;
int e;

x = xx;
/* Test for domain */
if( x <= 0.0f )
	{
	if( x == 0.0f )
		mtherr( fname, SING );
	else
		mtherr( fname, DOMAIN );
	return( -MAXL10 );
	}

/* separate mantissa from exponent */

x = frexp( x, &e );

/* logarithm using log(1+x) = x - .5x**2 + x**3 P(x) */

if( x < SQRTH )
	{
	e -= 1;
	x = 2.0f*x - 1.0f;
	}	
else
	{
	x = x - 1.0f;
	}


/* rational form */
z = x*x;
y = x * ( z * polevl( x, P, 8 ) );
y = y - 0.5f * z;   /*  y - 0.5 * x**2  */

/* multiply log of fraction by log10(e)
 * and base 2 exponent by log10(2)
 */
z = (x + y) * L10EB;  /* accumulate terms in order of size */
z += y * L10EA;
z += x * L10EA;
x = e;
z += x * L102B;
z += x * L102A;


return( z );
}
