/*							cbrt.c
 *
 *	Cube root
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, cbrt();
 *
 * y = cbrt( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the cube root of the argument, which may be negative.
 *
 * Range reduction involves determining the power of 2 of
 * the argument.  A polynomial of degree 2 applied to the
 * mantissa, and multiplication by the cube root of 1, 2, or 4
 * approximates the root to within about 0.1%.  Then Newton's
 * iteration is used to converge to an accurate result.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0,1e38      100000      7.6e-8      2.7e-8
 *
 */
/*							cbrt.c  */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "mconf.h"

static float CBRT2 = 1.25992104989487316477f;
static float CBRT4 = 1.58740105196819947475f;


#ifdef ANSIC
float frexp(float, int *), ldexp(float, int);

float cbrt( float xx )
#else
float frexp(), ldexp();

float cbrt(xx)
double xx;
#endif
{
int e, rem, sign;
float x, z;

x = xx;
if( x == 0 )
	return( 0.0f );
if( x > 0 )
	sign = 1;
else
	{
	sign = -1;
	x = -x;
	}

z = x;
/* extract power of 2, leaving
 * mantissa between 0.5 and 1
 */
x = frexp( x, &e );

/* Approximate cube root of number between .5 and 1,
 * peak relative error = 9.2e-6
 */
x = (((-0.13466110473359520655053f * x
      + 0.54664601366395524503440f ) * x
      - 0.95438224771509446525043f ) * x
      + 1.1399983354717293273738f  ) * x
      + 0.40238979564544752126924f;

/* exponent divided by 3 */
if( e >= 0 )
	{
	rem = e;
	e /= 3;
	rem -= 3*e;
	if( rem == 1 )
		x *= CBRT2;
	else if( rem == 2 )
		x *= CBRT4;
	}


/* argument less than 1 */

else
	{
	e = -e;
	rem = e;
	e /= 3;
	rem -= 3*e;
	if( rem == 1 )
		x /= CBRT2;
	else if( rem == 2 )
		x /= CBRT4;
	e = -e;
	}

/* multiply by power of 2 */
x = ldexp( x, e );

/* Newton iteration */
x -= ( x - (z/(x*x)) ) * 0.333333333333f;

if( sign < 0 )
	x = -x;
return(x);
}
