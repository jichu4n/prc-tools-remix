/*							struve.c
 *
 *      Struve function
 *
 *
 *
 * SYNOPSIS:
 *
 * float v, x, y, struve();
 *
 * y = struve( v, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Computes the Struve function Hv(x) of order v, argument x.
 * Negative x is rejected unless v is an integer.
 *
 * This module also contains the hypergeometric functions 1F2
 * and 3F0 and a routine for the Bessel function Yv(x) with
 * noninteger v.
 *
 *
 *
 * ACCURACY:
 *
 *  v varies from 0 to 10.
 *    Absolute error (relative error when |Hv(x)| > 1):
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,10      100000      9.0e-5      4.0e-6
 *
 */


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987, 1989 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"
#define DEBUG 0

extern float MACHEPF, MAXNUMF, PIF;

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float gamma(float), pow(float, float), sqrt(float);
float yv(float, float);
float floor(float), yn(int, float);
float jv(float, float);
float sin(float), cos(float);
#else
float gamma(), pow(), sqrt(), yv();
float floor(), yn(), jv(), sin(), cos();
#endif

#ifdef ANSIC
float onef2( float aa, float bb, float cc, float xx, float *err )
#else
float onef2( aa, bb, cc, xx, err )
double aa, bb, cc, xx;
float *err;
#endif
{
float a, b, c, x, n, a0, sum, t;
float an, bn, cn, max, z;

a = aa;
b = bb;
c = cc;
x = xx;
an = a;
bn = b;
cn = c;
a0 = 1.0f;
sum = 1.0f;
n = 1.0f;
t = 1.0f;
max = 0.0f;

do
	{
	if( an == 0 )
		goto done;
	if( bn == 0 )
		goto error;
	if( cn == 0 )
		goto error;
	if( (a0 > 1.0e34) || (n > 200) )
		goto error;
	a0 *= (an * x) / (bn * cn * n);
	sum += a0;
	an += 1.0f;
	bn += 1.0f;
	cn += 1.0f;
	n += 1.0f;
	z = fabsf( a0 );
	if( z > max )
		max = z;
	if( sum != 0 )
		t = fabsf( a0 / sum );
	else
		t = z;
	}
while( t > MACHEPF );

done:

*err = fabsf( MACHEPF*max /sum );

#if DEBUG
	printf(" onef2 cancellation error %.5E\n", *err );
#endif

goto xit;

error:
#if DEBUG
printf("onef2 does not converge\n");
#endif
*err = MAXNUMF;

xit:

#if DEBUG
printf("onef2( %.2E %.2E %.2E %.5E ) =  %.3E  %.6E\n", a, b, c, x, n, sum);
#endif
return(sum);
}



#ifdef ANSIC
float threef0( float aa, float bb, float cc, float xx, float *err )
#else
float threef0( aa, bb, cc, xx, err )
double aa, bb, cc, xx;
float *err;
#endif
{
float a, b, c, x, n, a0, sum, t, conv, conv1;
float an, bn, cn, max, z;

a = aa;
b = bb;
c = cc;
x = xx;
an = a;
bn = b;
cn = c;
a0 = 1.0f;
sum = 1.0f;
n = 1.0f;
t = 1.0f;
max = 0.0f;
conv = 1.0e38f;
conv1 = conv;

do
	{
	if( an == 0.0f )
		goto done;
	if( bn == 0.0f )
		goto done;
	if( cn == 0.0f )
		goto done;
	if( (a0 > 1.0e34f) || (n > 200) )
		goto error;
	a0 *= (an * bn * cn * x) / n;
	an += 1.0f;
	bn += 1.0f;
	cn += 1.0f;
	n += 1.0f;
	z = fabsf( a0 );
	if( z > max )
		max = z;
	if( z >= conv )
		{
		if( (z < max) && (z > conv1) )
			goto done;
		}
	conv1 = conv;
	conv = z;
	sum += a0;
	if( sum != 0 )
		t = fabsf( a0 / sum );
	else
		t = z;
	}
while( t > MACHEPF );

done:

t = fabsf( MACHEPF*max/sum );
#if DEBUG
	printf(" threef0 cancellation error %.5E\n", t );
#endif

max = fabsf( conv/sum );
if( max > t )
	t = max;
#if DEBUG
	printf(" threef0 convergence %.5E\n", max );
#endif

goto xit;

error:
#if DEBUG
printf("threef0 does not converge\n");
#endif
t = MAXNUMF;

xit:

#if DEBUG
printf("threef0( %.2E %.2E %.2E %.5E ) =  %.3E  %.6E\n", a, b, c, x, n, sum);
#endif

*err = t;
return(sum);
}




#ifdef ANSIC
float struve( float vv, float xx )
#else
float struve( vv, xx )
double vv, xx;
#endif
{
float v, x, y, ya, f, g, h, t;
float onef2err, threef0err;

v = vv;
x = xx;
f = floor(v);
if( (v < 0) && ( v-f == 0.5f ) )
	{
	y = jv( -v, x );
	f = 1.0f - f;
	g =  2.0f * floor(0.5f*f);
	if( g != f )
		y = -y;
	return(y);
	}
t = 0.25f*x*x;
f = fabsf(x);
g = 1.5f * fabsf(v);
if( (f > 30.0f) && (f > g) )
	{
	onef2err = MAXNUMF;
	y = 0.0f;
	}
else
	{
	y = onef2( 1.0f, 1.5f, 1.5f+v, -t, &onef2err );
	}

if( (f < 18.0f) || (x < 0.0f) )
	{
	threef0err = MAXNUMF;
	ya = 0.0f;
	}
else
	{
	ya = threef0( 1.0f, 0.5f, 0.5f-v, -1.0f/t, &threef0err );
	}

f = sqrt( PIF );
h = pow( 0.5f*x, v-1.0f );

if( onef2err <= threef0err )
	{
	g = gamma( v + 1.5f );
	y = y * h * t / ( 0.5f * f * g );
	return(y);
	}
else
	{
	g = gamma( v + 0.5f );
	ya = ya * h / ( f * g );
	ya = ya + yv( v, x );
	return(ya);
	}
}




/* Bessel function of noninteger order
 */

#ifdef ANSIC
float yv( float vv, float xx )
#else
float yv( vv, xx )
double vv, xx;
#endif
{
float v, x,  y, t;
int n;

v = vv;
x = xx;
y = floor( v );
if( y == v )
	{
	n = v;
	y = yn( n, x );
	return( y );
	}
t = PIF * v;
y = (cos(t) * jv( v, x ) - jv( -v, x ))/sin(t);
return( y );
}

/* Crossover points between ascending series and asymptotic series
 * for Struve function
 *
 *	 v	 x
 * 
 *	 0	19.2
 *	 1	18.95
 *	 2	19.15
 *	 3	19.3
 *	 5	19.7
 *	10	21.35
 *	20	26.35
 *	30	32.31
 *	40	40.0
 */
