/*							cosh.c
 *
 *	Hyperbolic cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, cosh();
 *
 * y = cosh( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns hyperbolic cosine of argument in the range MINLOGF to
 * MAXLOGF.
 *
 * cosh(x)  =  ( exp(x) + exp(-x) )/2.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE     +-MAXLOGF    100000      1.2e-7      2.8e-8
 *
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * cosh overflow  |x| > MAXLOGF       MAXNUMF
 *
 *
 */

/*							cosh.c */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1985, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"
extern float MAXLOGF, MAXNUMF;

#ifdef ANSIC
float exp(float);

float cosh(float xx)
#else
float exp();

float cosh(xx)
double xx;
#endif
{
float x, y;

x = xx;
if( x < 0 )
	x = -x;
if( x > MAXLOGF )
	{
	mtherr( "cosh", OVERFLOW );
	return( MAXNUMF );
	}	
y = exp(x);
y = y + 1.0f/y;
return( 0.5f*y );
}
