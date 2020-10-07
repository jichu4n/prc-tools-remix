/*							ellik.c
 *
 *	Incomplete elliptic integral of the first kind
 *
 *
 *
 * SYNOPSIS:
 *
 * float phi, m, y, ellik();
 *
 * y = ellik( phi, m );
 *
 *
 *
 * DESCRIPTION:
 *
 * Approximates the integral
 *
 *
 *
 *                phi
 *                 -
 *                | |
 *                |           dt
 * F(phi\m)  =    |    ------------------
 *                |                   2
 *              | |    sqrt( 1 - m sin t )
 *               -
 *                0
 *
 * of amplitude phi and modulus m, using the arithmetic -
 * geometric mean algorithm.
 *
 *
 *
 *
 * ACCURACY:
 *
 * Tested at random points with phi in [0, 2] and m in
 * [0, 1].
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0,2         10000       2.9e-7      5.8e-8
 *
 *
 */


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/*	Incomplete elliptic integral of first kind	*/

#include "mconf.h"
extern float PIF, PIO2F, MACHEPF;

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float sqrt(float), log(float), sin(float), tan(float), atan(float);
#else
float sqrt(), log(), sin(), tan(), atan();
#endif


#ifdef ANSIC
float ellik( float phia, float ma )
#else
float ellik( phia, ma )
double phia, ma;
#endif
{
float phi, m, a, b, c, temp;
float t;
int d, mod, sign;

phi = phia;
m = ma;
if( m == 0.0f )
	return( phi );
if( phi < 0.0f )
	{
	phi = -phi;
	sign = -1;
	}
else
	sign = 0;
a = 1.0f;
b = 1.0f - m;
if( b == 0.0f )
	return(  log(  tan( 0.5f*(PIO2F + phi) )  )   );
b = sqrt(b);
c = sqrt(m);
d = 1;
t = tan( phi );
mod = (phi + PIO2F)/PIF;

while( fabsf(c/a) > MACHEPF )
	{
	temp = b/a;
	phi = phi + atan(t*temp) + mod * PIF;
	mod = (phi + PIO2F)/PIF;
	t = t * ( 1.0f + temp )/( 1.0f - temp * t * t );
	c = ( a - b )/2.0f;
	temp = sqrt( a * b );
	a = ( a + b )/2.0f;
	b = temp;
	d += d;
	}

temp = (atan(t) + mod * PIF)/(d * a);
if( sign < 0 )
	temp = -temp;
return( temp );
}
