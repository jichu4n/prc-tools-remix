/*							beta.c
 *
 *	Beta function
 *
 *
 *
 * SYNOPSIS:
 *
 * float a, b, y, beta();
 *
 * y = beta( a, b );
 *
 *
 *
 * DESCRIPTION:
 *
 *                   -     -
 *                  | (a) | (b)
 * beta( a, b )  =  -----------.
 *                     -
 *                    | (a+b)
 *
 * For large arguments the logarithm of the function is
 * evaluated using lgam(), then exponentiated.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,30       10000       4.0e-5      6.0e-6
 *    IEEE       -20,0      10000       4.9e-3      5.4e-5
 *
 * ERROR MESSAGES:
 *
 *   message         condition          value returned
 * beta overflow   log(beta) > MAXLOG       0.0
 *                  a or b <0 integer        0.0
 *
 */

/*							beta.c	*/


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

#define fabsf(x) ( (x) < 0.0f ? -(x) : (x) )

#define MAXGAM 34.84425627277176174f


extern float MAXLOGF, MAXNUMF;
extern int sgngamf;

#ifdef ANSIC
float gamma(float), lgam(float), exp(float), floor(float);
#else
float gamma(), lgam(), exp(), floor();
#endif

#ifdef ANSIC
float beta( float aa, float bb )
#else
float beta( aa, bb )
double aa, bb;
#endif
{
float a, b, y;
int sign;

sign = 1;
a = aa;
b = bb;
if( a <= 0.0f )
	{
	if( a == floor(a) )
		goto over;
	}
if( b <= 0.0f )
	{
	if( b == floor(b) )
		goto over;
	}


y = a + b;
if( fabsf(y) > MAXGAM )
	{
	y = lgam(y);
	sign *= sgngamf; /* keep track of the sign */
	y = lgam(b) - y;
	sign *= sgngamf;
	y = lgam(a) + y;
	sign *= sgngamf;
	if( y > MAXLOGF )
		{
over:
		mtherr( "beta", OVERFLOW );
		return( sign * MAXNUMF );
		}
	return( sign * exp(y) );
	}

y = gamma(y);
if( y == 0.0f )
	goto over;

if( a > b )
	{
	y = gamma(a)/y;
	y *= gamma(b);
	}
else
	{
	y = gamma(b)/y;
	y *= gamma(a);
	}

return(y);
}
