/*							tanh.c
 *
 *	Hyperbolic tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, tanh();
 *
 * y = tanh( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns hyperbolic tangent of argument in the range MINLOG to
 * MAXLOG.
 *
 * A polynomial approximation is used for |x| < 0.625.
 * Otherwise,
 *
 *    tanh(x) = sinh(x)/cosh(x) = 1  -  2/(exp(2x) + 1).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -2,2        100000      1.3e-7      2.6e-8
 *
 */

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1989, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/* Single precision hyperbolic tangent
 * test interval: [-0.625, +0.625]
 * trials: 10000
 * peak relative error: 7.2e-8
 * rms relative error: 2.6e-8
 */
#include "mconf.h"

extern float MAXLOGF;

#ifdef ANSIC
float exp( float );

float tanh( float xx )
#else
float exp();

float tanh(xx)
double xx;
#endif
{
float x, z;

if( xx < 0 )
	x = -xx;
else
	x = xx;

if( x > 0.5f * MAXLOGF )
	{
	if( xx > 0 )
		return( 1.0f );
	else
		return( -1.0f );
	}
if( x >= 0.625f )
	{
	x = exp(x+x);
	z =  1.0f  - 2.0f/(x + 1.0f);
	if( xx < 0 )
		z = -z;
	}
else
	{
	z = x * x;
	z =
	(((( -5.70498872745E-3f * z
	  + 2.06390887954E-2f) * z
	  - 5.37397155531E-2f) * z
	  + 1.33314422036E-1f) * z
	  - 3.33332819422E-1f) * z * xx
	  + xx;
	}
return( z );
}
