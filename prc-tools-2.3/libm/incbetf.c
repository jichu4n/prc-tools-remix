/*							incbet.c
 *
 *	Incomplete beta integral
 *
 *
 * SYNOPSIS:
 *
 * float a, b, x, y, incbet();
 *
 * y = incbet( a, b, x );
 *
 *
 * DESCRIPTION:
 *
 * Returns incomplete beta integral of the arguments, evaluated
 * from zero to x.  The function is defined as
 *
 *                  x
 *     -            -
 *    | (a+b)      | |  a-1     b-1
 *  -----------    |   t   (1-t)   dt.
 *   -     -     | |
 *  | (a) | (b)   -
 *                 0
 *
 * The domain of definition is 0 <= x <= 1.  In this
 * implementation a and b are restricted to positive values.
 * The integral from x to 1 may be obtained by the symmetry
 * relation
 *
 *    1 - incbet( a, b, x )  =  incbet( b, a, 1-x ).
 *
 * The integral is evaluated by a continued fraction expansion.
 * If a < 1, the function calls itself recursively after a
 * transformation to increase a to a+1.
 *
 * ACCURACY:
 *
 * Tested at random points (a,b,x) with a and b in the indicated
 * interval and x between 0 and 1.
 *
 * arithmetic   domain     # trials      peak         rms
 * Relative error:
 *    IEEE       0,30       10000       3.7e-5      5.1e-6
 *    IEEE       0,100      10000       1.7e-4      2.5e-5
 * The useful domain for relative error is limited by underflow
 * of the single precision exponential function.
 * Absolute error:
 *    IEEE       0,30      100000       2.2e-5      9.6e-7
 *    IEEE       0,100      10000       6.5e-5      3.7e-6
 *
 * Larger errors may occur for extreme ratios of a and b.
 *
 * ERROR MESSAGES:
 *   message         condition      value returned
 * incbet domain     x<0, x>1          0.0
 */


/*
Cephes Math Library, Release 2.2:  July, 1992
Copyright 1984, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

#ifdef ANSIC
float lgam(float), exp(float), log(float);
static float incbdf(float, float, float);
static float incbcff(float, float, float);
float incbps(float, float, float);
#else
float lgam(), exp(), log();
float incbps();
static float incbcff(), incbdf();
#endif

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

/* BIG = 1/MACHEPF */
#define BIG   16777216.
extern float MACHEPF, MAXLOGF;
#define MINLOGF (-MAXLOGF)

#ifdef ANSIC
float incbet( float aaa, float bbb, float xxx )
#else
float incbet( aaa, bbb, xxx )
double aaa, bbb, xxx;
#endif
{
float aa, bb, xx, ans, a, b, t, x, onemx;
int flag;

aa = aaa;
bb = bbb;
xx = xxx;
if( (xx <= 0.0f) || ( xx >= 1.0f) )
	{
	if( xx == 0.0f )
		return(0.0f);
	if( xx == 1.0f )
		return( 1.0f );
	mtherr( "incbet", DOMAIN );
	return( 0.0f );
	}

onemx = 1.0f - xx;


/* transformation for small aa */

if( aa <= 1.0f )
	{
	ans = incbet( aa+1.0f, bb, xx );
	t = aa*log(xx) + bb*log( 1.0f-xx )
		+ lgam(aa+bb) - lgam(aa+1.0f) - lgam(bb);
	if( t > MINLOGF )
		ans += exp(t);
	return( ans );
	}


/* see if x is greater than the mean */

if( xx > (aa/(aa+bb)) )
	{
	flag = 1;
	a = bb;
	b = aa;
	t = xx;
	x = onemx;
	}
else
	{
	flag = 0;
	a = aa;
	b = bb;
	t = onemx;
	x = xx;
	}

/* transformation for small aa */
/*
if( a <= 1.0f )
	{
	ans = a*log(x) + b*log( onemx )
		+ lgam(a+b) - lgam(a+1.0f) - lgam(b);
 	t = incbet( a+1.0f, b, x );
	if( ans > MINLOGF )
		t += exp(ans);
	goto bdone;
	}
*/
/* Choose expansion for optimal convergence */


if( b > 10.0f )
	{
if( fabsf(b*x/a) < 0.3f )
	{
	t = incbps( a, b, x );
	goto bdone;
	}
	}

ans = x * (a+b-2.0f)/(a-1.0f);
if( ans < 1.0f )
	{
	ans = incbcff( a, b, x );
	t = b * log( t );
	}
else
	{
	ans = incbdf( a, b, x );
	t = (b-1.0f) * log(t);
	}

t += a*log(x) + lgam(a+b) - lgam(a) - lgam(b);
t += log( ans/a );

if( t < MINLOGF )
	{
	t = 0.0f;
	if( flag == 0 )
		{
		mtherr( "incbet", UNDERFLOW );
		}
	}
else
	{
	t = exp(t);
	}
bdone:

if( flag )
	t = 1.0f - t;

return( t );
}

/* Continued fraction expansion #1
 * for incomplete beta integral
 */

#ifdef ANSIC
static float incbcff( float aa, float bb, float xx )
#else
static float incbcff( aa, bb, xx )
double aa, bb, xx;
#endif
{
float a, b, x, xk, pk, pkm1, pkm2, qk, qkm1, qkm2;
float k1, k2, k3, k4, k5, k6, k7, k8;
float r, t, ans;
static float big = BIG;
int n;

a = aa;
b = bb;
x = xx;
k1 = a;
k2 = a + b;
k3 = a;
k4 = a + 1.0f;
k5 = 1.0f;
k6 = b - 1.0f;
k7 = k4;
k8 = a + 2.0f;

pkm2 = 0.0f;
qkm2 = 1.0f;
pkm1 = 1.0f;
qkm1 = 1.0f;
ans = 1.0f;
r = 0.0f;
n = 0;
do
	{
	
	xk = -( x * k1 * k2 )/( k3 * k4 );
	pk = pkm1 +  pkm2 * xk;
	qk = qkm1 +  qkm2 * xk;
	pkm2 = pkm1;
	pkm1 = pk;
	qkm2 = qkm1;
	qkm1 = qk;

	xk = ( x * k5 * k6 )/( k7 * k8 );
	pk = pkm1 +  pkm2 * xk;
	qk = qkm1 +  qkm2 * xk;
	pkm2 = pkm1;
	pkm1 = pk;
	qkm2 = qkm1;
	qkm1 = qk;

	if( qk != 0 )
		r = pk/qk;
	if( r != 0 )
		{
		t = fabsf( (ans - r)/r );
		ans = r;
		}
	else
		t = 1.0f;

	if( t < MACHEPF )
		goto cdone;

	k1 += 1.0f;
	k2 += 1.0f;
	k3 += 2.0f;
	k4 += 2.0f;
	k5 += 1.0f;
	k6 -= 1.0f;
	k7 += 2.0f;
	k8 += 2.0f;

	if( (fabsf(qk) + fabsf(pk)) > big )
		{
		pkm2 *= MACHEPF;
		pkm1 *= MACHEPF;
		qkm2 *= MACHEPF;
		qkm1 *= MACHEPF;
		}
	if( (fabsf(qk) < MACHEPF) || (fabsf(pk) < MACHEPF) )
		{
		pkm2 *= big;
		pkm1 *= big;
		qkm2 *= big;
		qkm1 *= big;
		}
	}
while( ++n < 100 );

cdone:
return(ans);
}


/* Continued fraction expansion #2
 * for incomplete beta integral
 */

#ifdef ANSIC
static float incbdf( float aa, float bb, float xx )
#else
static float incbdf( aa, bb, xx )
double aa, bb, xx;
#endif
{
float a, b, x, xk, pk, pkm1, pkm2, qk, qkm1, qkm2;
float k1, k2, k3, k4, k5, k6, k7, k8;
float r, t, ans, z;
static float big = BIG;
int n;

a = aa;
b = bb;
x = xx;
k1 = a;
k2 = b - 1.0f;
k3 = a;
k4 = a + 1.0f;
k5 = 1.0f;
k6 = a + b;
k7 = a + 1.0f;;
k8 = a + 2.0f;

pkm2 = 0.0f;
qkm2 = 1.0f;
pkm1 = 1.0f;
qkm1 = 1.0f;
z = x / (1.0f-x);
ans = 1.0f;
r = 0.0f;
n = 0;
do
	{
	
	xk = -( z * k1 * k2 )/( k3 * k4 );
	pk = pkm1 +  pkm2 * xk;
	qk = qkm1 +  qkm2 * xk;
	pkm2 = pkm1;
	pkm1 = pk;
	qkm2 = qkm1;
	qkm1 = qk;

	xk = ( z * k5 * k6 )/( k7 * k8 );
	pk = pkm1 +  pkm2 * xk;
	qk = qkm1 +  qkm2 * xk;
	pkm2 = pkm1;
	pkm1 = pk;
	qkm2 = qkm1;
	qkm1 = qk;

	if( qk != 0 )
		r = pk/qk;
	if( r != 0 )
		{
		t = fabsf( (ans - r)/r );
		ans = r;
		}
	else
		t = 1.0f;

	if( t < MACHEPF )
		goto cdone;

	k1 += 1.0f;
	k2 -= 1.0f;
	k3 += 2.0f;
	k4 += 2.0f;
	k5 += 1.0f;
	k6 += 1.0f;
	k7 += 2.0f;
	k8 += 2.0f;

	if( (fabsf(qk) + fabsf(pk)) > big )
		{
		pkm2 *= MACHEPF;
		pkm1 *= MACHEPF;
		qkm2 *= MACHEPF;
		qkm1 *= MACHEPF;
		}
	if( (fabsf(qk) < MACHEPF) || (fabsf(pk) < MACHEPF) )
		{
		pkm2 *= big;
		pkm1 *= big;
		qkm2 *= big;
		qkm1 *= big;
		}
	}
while( ++n < 100 );

cdone:
return(ans);
}


/* power series */
#ifdef ANSIC
float incbps( float aa, float bb, float xx )
#else
float incbps( aa, bb, xx )
double aa, bb, xx;
#endif
{
float a, b, x, t, u, y, s;

a = aa;
b = bb;
x = xx;

y = a * log(x) + (b-1.0f)*log(1.0f-x) - log(a);
y -= lgam(a) + lgam(b);
y += lgam(a+b);


t = x / (1.0f - x);
s = 0.0f;
u = 1.0f;
do
	{
	b -= 1.0f;
	if( b == 0.0f )
		break;
	a += 1.0f;
	u *= t*b/a;
	s += u;
	}
while( fabsf(u) > MACHEPF );

if( y < MINLOGF )
	{
	mtherr( "incbet", UNDERFLOW );
	s = 0.0f;
	}
else
	s = exp(y) * (1.0f + s);
/*printf( "incbps: %.4e\n", s );*/
return(s);
}
