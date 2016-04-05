/*							iv.c
 *
 *	Modified Bessel function of noninteger order
 *
 *
 *
 * SYNOPSIS:
 *
 * float v, x, y, iv();
 *
 * y = iv( v, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns modified Bessel function of order v of the
 * argument.  If x is negative, v must be integer valued.
 *
 * The function is defined as Iv(x) = Jv( ix ).  It is
 * here computed in terms of the confluent hypergeometric
 * function, according to the formula
 *
 *              v  -x
 * Iv(x) = (x/2)  e   hyperg( v+0.5, 2v+1, 2x ) / gamma(v+1)
 *
 * If v is a negative integer, then v is replaced by -v.
 *
 *
 * ACCURACY:
 *
 * Tested at random points (v, x), with v between 0 and
 * 30, x between 0 and 28.
 * arithmetic   domain     # trials      peak         rms
 *                      Relative error:
 *    IEEE      0,15          3000      4.7e-6      5.4e-7
 *          Absolute error (relative when function > 1)
 *    IEEE      0,30          5000      8.5e-6      1.3e-6
 *
 * Accuracy is diminished if v is near a negative integer.
 * The useful domain for relative error is limited by overflow
 * of the single precision exponential function.
 *
 * See also hyperg.c.
 *
 */
/*							iv.c	*/
/*	Modified Bessel function of noninteger order		*/
/* If x < 0, then v must be an integer. */


/*
Cephes Math Library Release 2.2: June, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "mconf.h"

extern double MAXNUMF;
#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float hyperg(float, float, float);
float exp(float), gamma(float), log(float), floor(float);

float iv( float v, float x )
#else
float hyperg(), exp(), gamma(), log(), floor();

float iv( v, x )
double v, x;
#endif
{
int sign;
float t, ax;

/* If v is a negative integer, invoke symmetry */
t = floor(v);
if( v < 0.0f )
	{
	if( t == v )
		{
		v = -v;	/* symmetry */
		t = -t;
		}
	}
/* If x is negative, require v to be an integer */
sign = 1;
if( x < 0.0f )
	{
	if( t != v )
		{
		mtherr( "iv", DOMAIN );
		return( 0.0f );
		}
	if( v != 2.0f * floor(v/2.0f) )
		sign = -1;
	}

/* Avoid logarithm singularity */
if( x == 0.0f )
	{
	if( v == 0.0f )
		return( 1.0f );
	if( v < 0.0f )
		{
		mtherr( "iv", OVERFLOW );
		return( MAXNUMF );
		}
	else
		return( 0.0f );
	}

ax = fabsf(x);
t = v * log( 0.5f * ax )  -  x;
t = sign * exp(t) / gamma( v + 1.0f );
ax = v + 0.5f;
return( t * hyperg( ax,  2.0f * ax,  2.0f * x ) );
}
