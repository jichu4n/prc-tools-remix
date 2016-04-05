/*							igam.c
 *
 *	Incomplete gamma integral
 *
 *
 *
 * SYNOPSIS:
 *
 * float a, x, y, igam();
 *
 * y = igam( a, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * The function is defined by
 *
 *                           x
 *                            -
 *                   1       | |  -t  a-1
 *  igam(a,x)  =   -----     |   e   t   dt.
 *                  -      | |
 *                 | (a)    -
 *                           0
 *
 *
 * In this implementation both arguments must be positive.
 * The integral is evaluated by either a power series or
 * continued fraction expansion, depending on the relative
 * values of a and x.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0,30        20000       7.8e-6      5.9e-7
 *
 */
/*							igamc()
 *
 *	Complemented incomplete gamma integral
 *
 *
 *
 * SYNOPSIS:
 *
 * float a, x, y, igamc();
 *
 * y = igamc( a, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * The function is defined by
 *
 *
 *  igamc(a,x)   =   1 - igam(a,x)
 *
 *                            inf.
 *                              -
 *                     1       | |  -t  a-1
 *               =   -----     |   e   t   dt.
 *                    -      | |
 *                   | (a)    -
 *                             x
 *
 *
 * In this implementation both arguments must be positive.
 * The integral is evaluated by either a power series or
 * continued fraction expansion, depending on the relative
 * values of a and x.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0,30        30000       7.8e-6      5.9e-7
 *
 */

/*
Cephes Math Library Release 2.2: June, 1992
Copyright 1985, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

/* BIG = 1/MACHEPF */
#define BIG   16777216.

extern float MACHEPF, MAXLOGF;

#ifdef ANSIC
float lgam(float), exp(float), log(float), igam(float, float);
#else
float lgam(), exp(), log(), igam();
#endif

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )



#ifdef ANSIC
float igamc( float aa, float xx )
#else
float igamc( aa, xx )
double aa, xx;
#endif
{
float a, x, ans, c, yc, ax, y, z;
float pk, pkm1, pkm2, qk, qkm1, qkm2;
float r, t;
static float big = BIG;

a = aa;
x = xx;
if( (x <= 0.0f) || ( a <= 0.0f) )
	return( 1.0f );

if( (x < 1.0f) || (x < a) )
	return( 1.0f - igam(a,x) );

ax = a * log(x) - x - lgam(a);
if( ax < -MAXLOGF )
	{
	mtherr( "igamc", UNDERFLOW );
	return( 0.0f );
	}
ax = exp(ax);

/* continued fraction */
y = 1.0f - a;
z = x + y + 1.0f;
c = 0.0f;
pkm2 = 1.0f;
qkm2 = x;
pkm1 = x + 1.0f;
qkm1 = z * x;
ans = pkm1/qkm1;

do
	{
	c += 1.0f;
	y += 1.0f;
	z += 2.0f;
	yc = y * c;
	pk = pkm1 * z  -  pkm2 * yc;
	qk = qkm1 * z  -  qkm2 * yc;
	if( qk != 0 )
		{
		r = pk/qk;
		t = fabsf( (ans - r)/r );
		ans = r;
		}
	else
		t = 1.0f;
	pkm2 = pkm1;
	pkm1 = pk;
	qkm2 = qkm1;
	qkm1 = qk;
	if( fabsf(pk) > big )
		{
		pkm2 *= MACHEPF;
		pkm1 *= MACHEPF;
		qkm2 *= MACHEPF;
		qkm1 *= MACHEPF;
		}
	}
while( t > MACHEPF );

return( ans * ax );
}



/* left tail of incomplete gamma function:
 *
 *          inf.      k
 *   a  -x   -       x
 *  x  e     >   ----------
 *           -     -
 *          k=0   | (a+k+1)
 *
 */

#ifdef ANSIC
float igam( float aa, float xx )
#else
float igam( aa, xx )
double aa, xx;
#endif
{
float a, x, ans, ax, c, r;

a = aa;
x = xx;
if( (x <= 0.0f) || ( a <= 0.0f) )
	return( 0.0f );

if( (x > 1.0f) && (x > a ) )
	return( 1.0f - igamc(a,x) );

/* Compute  x**a * exp(-x) / gamma(a)  */
ax = a * log(x) - x - lgam(a);
if( ax < -MAXLOGF )
	{
	mtherr( "igam", UNDERFLOW );
	return( 0.0f );
	}
ax = exp(ax);

/* power series */
r = a;
c = 1.0f;
ans = 1.0f;

do
	{
	r += 1.0f;
	c *= x/r;
	ans += c;
	}
while( c/ans > MACHEPF );

return( ans * ax/a );
}
