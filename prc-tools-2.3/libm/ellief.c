/*							ellie.c
 *
 *	Incomplete elliptic integral of the second kind
 *
 *
 *
 * SYNOPSIS:
 *
 * float phi, m, y, ellie();
 *
 * y = ellie( phi, m );
 *
 *
 *
 * DESCRIPTION:
 *
 * Approximates the integral
 *
 *
 *                phi
 *                 -
 *                | |
 *                |                   2
 * E(phi\m)  =    |    sqrt( 1 - m sin t ) dt
 *                |
 *              | |    
 *               -
 *                0
 *
 * of amplitude phi and modulus m, using the arithmetic -
 * geometric mean algorithm.
 *
 *
 *
 * ACCURACY:
 *
 * Tested at random arguments with phi in [0, 2] and m in
 * [0, 1].
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,2        10000       4.5e-7      7.4e-8
 *
 *
 */


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/*	Incomplete elliptic integral of second kind	*/

#include "mconf.h"

extern float PIF, PIO2F, MACHEPF;

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float sqrt(float), log(float), sin(float), tan(float), atan(float);
float ellpe(float), ellpk(float);
#else
float sqrt(), log(), sin(), tan(), atan();
float ellpe(), ellpk();
#endif


#ifdef ANSIC
float ellie( float phia, float ma )
#else
float ellie( phia, ma )
double phia, ma;
#endif
{
float phi, m, a, b, c, e, temp;
float lphi, t;
int d, mod;

phi = phia;
m = ma;
if( m == 0.0f )
	return( phi );
if( m == 1.0f )
	return( sin(phi) );
lphi = phi;
if( lphi < 0.0f )
	lphi = -lphi;
a = 1.0f;
b = 1.0f - m;
b = sqrt(b);
c = sqrt(m);
d = 1;
e = 0.0f;
t = tan( lphi );
mod = (lphi + PIO2F)/PIF;

while( fabsf(c/a) > MACHEPF )
	{
	temp = b/a;
	lphi = lphi + atan(t*temp) + mod * PIF;
	mod = (lphi + PIO2F)/PIF;
	t = t * ( 1.0f + temp )/( 1.0f - temp * t * t );
	c = 0.5f * ( a - b );
	temp = sqrt( a * b );
	a = 0.5f * ( a + b );
	b = temp;
	d += d;
	e += c * sin(lphi);
	}

b = 1.0f - m;
temp = ellpe(b)/ellpk(b);
temp *= (atan(t) + mod * PIF)/(d * a);
temp += e;
if( phi < 0.0f )
	temp = -temp;
return( temp );
}
