/*							clog.c
 *
 *	Complex natural logarithm
 *
 *
 *
 * SYNOPSIS:
 *
 * void clog();
 * cmplxf z, w;
 *
 * clog( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns complex logarithm to the base e (2.718...) of
 * the complex argument x.
 *
 * If z = x + iy, r = sqrt( x**2 + y**2 ),
 * then
 *       w = log(r) + i arctan(y/x).
 * 
 * The arctangent ranges from -PI to +PI.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.9e-6       6.2e-8
 *
 * Larger relative error can be observed for z near 1 +i0.
 * In IEEE arithmetic the peak absolute error is 3.1e-7.
 *
 */

#include "mconf.h"
extern float MAXNUMF, MACHEPF, PIF, PIO2F;
#ifdef ANSIC
float cabs(cmplxf *), sqrt(float), log(float), atan2(float, float);
float exp(float), sin(float), cos(float);
float cosh(float), sinh(float), asin(float);
float ctans(cmplxf *), redupi(float);
void cchsh( float, float *, float * );
void cadd( cmplxf *, cmplxf *, cmplxf * );
void csqrt( cmplxf *, cmplxf * );
#else
float cabs(), sqrt(), log(), atan2();
float exp(), sin(), cos();
float cosh(), sinh(), asin();
float ctans(), redupi();
void cchsh(), csqrt(), cadd();
#endif

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

void clog( z, w )
register cmplxf *z, *w;
{
float p, rr;

/*rr = sqrt( z->r * z->r  +  z->i * z->i );*/
rr = cabs(z);
p = log(rr);
#if ANSIC
rr = atan2( z->i, z->r );
#else
rr = atan2( z->r, z->i );
if( rr > PIF )
	rr -= PIF + PIF;
#endif
w->i = rr;
w->r = p;
}
/*							cexp()
 *
 *	Complex exponential function
 *
 *
 *
 * SYNOPSIS:
 *
 * void cexp();
 * cmplxf z, w;
 *
 * cexp( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the exponential of the complex argument z
 * into the complex result w.
 *
 * If
 *     z = x + iy,
 *     r = exp(x),
 *
 * then
 *
 *     w = r cos y + i r sin y.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.4e-7      4.5e-8
 *
 */

void cexp( z, w )
register cmplxf *z, *w;
{
float r;

r = exp( z->r );
w->r = r * cos( z->i );
w->i = r * sin( z->i );
}
/*							csin()
 *
 *	Complex circular sine
 *
 *
 *
 * SYNOPSIS:
 *
 * void csin();
 * cmplxf z, w;
 *
 * csin( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *     w = sin x  cosh y  +  i cos x sinh y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.9e-7      5.5e-8
 *
 */

void csin( z, w )
register cmplxf *z, *w;
{
float ch, sh;

cchsh( z->i, &ch, &sh );
w->r = sin( z->r ) * ch;
w->i = cos( z->r ) * sh;
}



/* calculate cosh and sinh */

#ifdef ANSIC
void cchsh( float xx, float *c, float *s )
#else
void cchsh( xx, c, s )
double xx;
float *c, *s;
#endif
{
float x, e, ei;

x = xx;
if( fabsf(x) <= 0.5f )
	{
	*c = cosh(x);
	*s = sinh(x);
	}
else
	{
	e = exp(x);
	ei = 0.5f/e;
	e = 0.5f * e;
	*s = e - ei;
	*c = e + ei;
	}
}

/*							ccos()
 *
 *	Complex circular cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * void ccos();
 * cmplxf z, w;
 *
 * ccos( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *     w = cos x  cosh y  -  i sin x sinh y.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.8e-7       5.5e-8
 */

void ccos( z, w )
register cmplxf *z, *w;
{
float ch, sh;

cchsh( z->i, &ch, &sh );
w->r = cos( z->r ) * ch;
w->i = -sin( z->r ) * sh;
}
/*							ctan()
 *
 *	Complex circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void ctan();
 * cmplxf z, w;
 *
 * ctan( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *           sin 2x  +  i sinh 2y
 *     w  =  --------------------.
 *            cos 2x  +  cosh 2y
 *
 * On the real axis the denominator is zero at odd multiples
 * of PI/2.  The denominator is evaluated by its Taylor
 * series near these points.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       3.3e-7       5.1e-8
 */

void ctan( z, w )
register cmplxf *z, *w;
{
float d;

d = cos( 2.0f * z->r ) + cosh( 2.0f * z->i );

if( fabsf(d) < 0.25f )
	d = ctans(z);

if( d == 0.0f )
	{
	mtherr( "ctan", OVERFLOW );
	w->r = MAXNUMF;
	w->i = MAXNUMF;
	return;
	}

w->r = sin( 2.0f * z->r ) / d;
w->i = sinh( 2.0f * z->i ) / d;
}
/*							ccot()
 *
 *	Complex circular cotangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void ccot();
 * cmplxf z, w;
 *
 * ccot( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *
 *           sin 2x  -  i sinh 2y
 *     w  =  --------------------.
 *            cosh 2y  -  cos 2x
 *
 * On the real axis, the denominator has zeros at even
 * multiples of PI/2.  Near these points it is evaluated
 * by a Taylor series.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       3.6e-7       5.7e-8
 * Also tested by ctan * ccot = 1 + i0.
 */

void ccot( z, w )
register cmplxf *z, *w;
{
float d;


d = cosh(2.0f * z->i) - cos(2.0f * z->r);

if( fabsf(d) < 0.25f )
	d = ctans(z);

if( d == 0.0f )
	{
	mtherr( "ccot", OVERFLOW );
	w->r = MAXNUMF;
	w->i = MAXNUMF;
	return;
	}

d = 1.0f/d;
w->r = sin( 2.0f * z->r ) * d;
w->i = -sinh( 2.0f * z->i ) * d;
}

/* Program to subtract nearest integer multiple of PI */
/* extended precision value of PI: */

static double DP1 =  3.140625;
static double DP2 =  9.67502593994140625E-4;
static double DP3 =  1.509957990978376432E-7;


#ifdef ANSIC
float redupi(float xx)
#else
float redupi(xx)
double xx;
#endif
{
float x, t;
long i;

x = xx;
t = x/PIF;
if( t >= 0.0 )
	t += 0.5;
else
	t -= 0.5;

i = t;	/* the multiple */
t = i;
t = ((x - t * DP1) - t * DP2) - t * DP3;
return(t);
}

/*  Taylor series expansion for cosh(2y) - cos(2x)	*/

float ctans(z)
cmplxf *z;
{
float f, x, x2, y, y2, rn, t, d;

x = fabsf( 2.0f * z->r );
y = fabsf( 2.0f * z->i );

x = redupi(x);

x = x * x;
y = y * y;
x2 = 1.0f;
y2 = 1.0f;
f = 1.0f;
rn = 0.0f;
d = 0.0f;
do
	{
	rn += 1.0f;
	f *= rn;
	rn += 1.0f;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 + x2;
	t /= f;
	d += t;

	rn += 1.0f;
	f *= rn;
	rn += 1.0f;
	f *= rn;
	x2 *= x;
	y2 *= y;
	t = y2 - x2;
	t /= f;
	d += t;
	}
while( fabsf(t/d) > MACHEPF );
return(d);
}
/*							casin()
 *
 *	Complex circular arc sine
 *
 *
 *
 * SYNOPSIS:
 *
 * void casin();
 * cmplxf z, w;
 *
 * casin( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * Inverse complex sine:
 *
 *                               2
 * w = -i clog( iz + csqrt( 1 - z ) ).
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       1.1e-5      1.5e-6
 * Larger relative error can be observed for z near zero.
 *
 */

void casin( z, w )
cmplxf *z, *w;
{
float x, y;
static cmplxf ca, ct, zz, z2;
/*
float cn, n;
static float a, b, s, t, u, v, y2;
static cmplxf sum;
*/

x = z->r;
y = z->i;

if( y == 0.0f )
	{
	if( fabsf(x) > 1.0f )
		{
		w->r = PIO2F;
		w->i = 0.0f;
		mtherr( "casin", DOMAIN );
		}
	else
		{
		w->r = asin(x);
		w->i = 0.0f;
		}
	return;
	}

/* Power series expansion */
/*
b = cabs(z);
if( b < 0.125f )
{
z2.r = (x - y) * (x + y);
z2.i = 2.0f * x * y;

cn = 1.0f;
n = 1.0f;
ca.r = x;
ca.i = y;
sum.r = x;
sum.i = y;
do
	{
	ct.r = z2.r * ca.r  -  z2.i * ca.i;
	ct.i = z2.r * ca.i  +  z2.i * ca.r;
	ca.r = ct.r;
	ca.i = ct.i;

	cn *= n;
	n += 1.0f;
	cn /= n;
	n += 1.0f;
	b = cn/n;

	ct.r *= b;
	ct.i *= b;
	sum.r += ct.r;
	sum.i += ct.i;
	b = fabsf(ct.r) + fabsf(ct.i);
	}
while( b > MACHEPF );
w->r = sum.r;
w->i = sum.i;
return;
}
*/


ca.r = x;
ca.i = y;

ct.r = -ca.i;	/* iz */
ct.i = ca.r;

	/* sqrt( 1 - z*z) */
/* cmul( &ca, &ca, &zz ) */
zz.r = (ca.r - ca.i) * (ca.r + ca.i);	/*x * x  -  y * y */
zz.i = 2.0f * ca.r * ca.i;

zz.r = 1.0f - zz.r;
zz.i = -zz.i;
csqrt( &zz, &z2 );

cadd( &z2, &ct, &zz );
clog( &zz, &zz );
w->r = zz.i;	/* mult by 1/i = -i */
w->i = -zz.r;
return;
}
/*							cacos()
 *
 *	Complex circular arc cosine
 *
 *
 *
 * SYNOPSIS:
 *
 * void cacos();
 * cmplxf z, w;
 *
 * cacos( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 *
 * w = arccos z  =  PI/2 - arcsin z.
 *
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000       9.2e-6       1.2e-6
 *
 */

void cacos( z, w )
cmplxf *z, *w;
{

casin( z, w );
w->r = PIO2F  -  w->r;
w->i = -w->i;
}
/*							catan()
 *
 *	Complex circular arc tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * void catan();
 * cmplxf z, w;
 *
 * catan( &z, &w );
 *
 *
 *
 * DESCRIPTION:
 *
 * If
 *     z = x + iy,
 *
 * then
 *          1       (    2x     )
 * Re w  =  - arctan(-----------)  +  k PI
 *          2       (     2    2)
 *                  (1 - x  - y )
 *
 *               ( 2         2)
 *          1    (x  +  (y+1) )
 * Im w  =  - log(------------)
 *          4    ( 2         2)
 *               (x  +  (y-1) )
 *
 * Where k is an arbitrary integer.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10,+10     30000        2.3e-6      5.2e-8
 *
 */

void catan( z, w )
cmplxf *z, *w;
{
float a, t, x, x2, y;

x = z->r;
y = z->i;

if( (x == 0.0f) && (y > 1.0f) )
	goto ovrf;

x2 = x * x;
a = 1.0f - x2 - (y * y);
if( a == 0.0f )
	goto ovrf;

#if ANSIC
t = 0.5f * atan2( 2.0f * x, a );
#else
t = 0.5f * atan2( a, 2.0f * x );
#endif
w->r = redupi( t );

t = y - 1.0f;
a = x2 + (t * t);
if( a == 0.0f )
	goto ovrf;

t = y + 1.0f;
a = (x2 + (t * t))/a;
w->i = 0.25f*log(a);
return;

ovrf:
mtherr( "catan", OVERFLOW );
w->r = MAXNUMF;
w->i = MAXNUMF;
}
