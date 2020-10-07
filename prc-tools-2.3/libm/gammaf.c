/*							gamma.c
 *
 *	Gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, gamma();
 * extern int sgngamf;
 *
 * y = gamma( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns gamma function of the argument.  The result is
 * correctly signed, and the sign (+1 or -1) is also
 * returned in a global (extern) variable named sgngamf.
 * This same variable is also filled in by the logarithmic
 * gamma function lgam().
 *
 * Arguments between 0 and 10 are reduced by recurrence and the
 * function is approximated by a polynomial function covering
 * the interval (2,3).  Large arguments are handled by Stirling's
 * formula. Negative arguments are made positive using
 * a reflection formula.  
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,-33      100,000     5.7e-7      1.0e-7
 *    IEEE       -33,0      100,000     6.1e-7      1.2e-7
 *
 *
 */
/*							lgam()
 *
 *	Natural logarithm of gamma function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, lgam();
 * extern int sgngamf;
 *
 * y = lgam( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the base e (2.718...) logarithm of the absolute
 * value of the gamma function of the argument.
 * The sign (+1 or -1) of the gamma function is returned in a
 * global (extern) variable named sgngamf.
 *
 * For arguments greater than 6.5, the logarithm of the gamma
 * function is approximated by the logarithmic version of
 * Stirling's formula.  Arguments between 0 and +6.5 are reduced by
 * by recurrence to the interval [.75,1.25] or [1.5,2.5] of a rational
 * approximation.  The cosecant reflection formula is employed for
 * arguments less than zero.
 *
 * Arguments greater than MAXLGM = 2.035093e36 return MAXNUM and an
 * error message.
 *
 *
 *
 * ACCURACY:
 *
 *
 *
 * arithmetic      domain        # trials     peak         rms
 *    IEEE        -100,+100       500,000    7.4e-7       6.8e-8
 * The error criterion was relative when the function magnitude
 * was greater than one but absolute when it was less than one.
 * The routine has low relative error for positive arguments.
 *
 * The following test used the relative error criterion.
 *    IEEE    -2, +3              100000     4.0e-7      5.6e-8
 *
 */

/*							gamma.c	*/
/*	gamma function	*/

/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1989, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "mconf.h"

/* define MAXGAM 34.84425627277176174 */

/* Stirling's formula for the gamma function
 * gamma(x) = sqrt(2 pi) x^(x-.5) exp(-x) ( 1 + 1/x P(1/x) )
 * .028 < 1/x < .1
 * relative error < 1.9e-11
 */
static float STIR[] = {
-2.705194986674176E-003,
 3.473255786154910E-003,
 8.333331788340907E-002,
};
static float MAXSTIR = 26.77;
static float SQTPIF = 2.50662827463100050242; /* sqrt( 2 pi ) */

int sgngamf = 0;
extern int sgngamf;
extern float MAXLOGF, MAXNUMF, PIF;

#ifdef ANSIC
float exp(float);
float log(float);
float pow( float, float );
float sin(float);
float gamma(float);
float floor(float);
static float stirf(float);
float polevl( float, float *, int );
float p1evl( float, float *, int );
#else
float exp(), log(), pow(), sin(), floor();
float polevl(), p1evl();
static float stirf();
#endif

/* Gamma function computed by Stirling's formula,
 * sqrt(2 pi) x^(x-.5) exp(-x) (1 + 1/x P(1/x))
 * The polynomial STIR is valid for 33 <= x <= 172.
 */
#ifdef ANSIC
static float stirf( float xx )
#else
static float stirf(xx)
double xx;
#endif
{
float x, y, w, v;

x = xx;
w = 1.0f/x;
w = 1.0f + w * polevl( w, STIR, 2 );
y = exp( -x );
if( x > MAXSTIR )
	{ /* Avoid overflow in pow() */
	v = pow( x, 0.5f * x - 0.25f );
	y *= v;
	y *= v;
	}
else
	{
	y = pow( x, x - 0.5f ) * y;
	}
y = SQTPIF * y * w;
return( y );
}


/* gamma(x+2), 0 < x < 1 */
static float P[] = {
 1.536830450601906E-003,
 5.397581592950993E-003,
 4.130370201859976E-003,
 7.232307985516519E-002,
 8.203960091619193E-002,
 4.117857447645796E-001,
 4.227867745131584E-001,
 9.999999822945073E-001,
};

#ifdef ANSIC
float gamma( float xx )
#else
float gamma(xx)
double xx;
#endif
{
float p, q, x, z, nz;
int i, direction, negative;

x = xx;
sgngamf = 1;
negative = 0;
nz = 0.0f;
if( x < 0.0f )
	{
	negative = 1;
	q = -x;
	p = floor(q);
	if( p == q )
		goto goverf;
	i = p;
	if( (i & 1) == 0 )
		sgngamf = -1;
	nz = q - p;
	if( nz > 0.5f )
		{
		p += 1.0f;
		nz = q - p;
		}
	nz = q * sin( PIF * nz );
	if( nz == 0.0f )
		{
goverf:
		mtherr( "gamma", OVERFLOW );
		return( sgngamf * MAXNUMF);
		}
	if( nz < 0 )
		nz = -nz;
	x = q;
	}
if( x >= 10.0f )
	{
	z = stirf(x);
	}
if( x < 2.0f )
	direction = 1;
else
	direction = 0;
z = 1.0f;
while( x >= 3.0f )
	{
	x -= 1.0f;
	z *= x;
	}
/*
while( x < 0.0f )
	{
	if( x > -1.E-4 )
		goto small;
	z *=x;
	x += 1.0;
	}
*/
while( x < 2.0f )
	{
	if( x < 1.e-4 )
		goto small;
	z *=x;
	x += 1.0f;
	}

if( direction )
	z = 1.0f/z;

if( (x == 2.0f) || (x == 3.0f) )
	return(z);

x -= 2.0f;
p = z * polevl( x, P, 7 );

gdone:

if( negative )
	{
	p = sgngamf * PIF/(nz * p );
	}
return(p);

small:
if( x == 0.0f )
	{
	mtherr( "gamma", SING );
	return( MAXNUMF );
	}
else
	{
	p = z / ((1.0f + 0.5772156649015329f * x) * x);
	goto gdone;
	}
}




/* log gamma(x+2), -.5 < x < .5 */
static float B[] = {
 6.055172732649237E-004,
-1.311620815545743E-003,
 2.863437556468661E-003,
-7.366775108654962E-003,
 2.058355474821512E-002,
-6.735323259371034E-002,
 3.224669577325661E-001,
 4.227843421859038E-001
};

/* log gamma(x+1), -.25 < x < .25 */
static float C[] = {
 1.369488127325832E-001,
-1.590086327657347E-001,
 1.692415923504637E-001,
-2.067882815621965E-001,
 2.705806208275915E-001,
-4.006931650563372E-001,
 8.224670749082976E-001,
-5.772156501719101E-001
};

/* log( sqrt( 2*pi ) ) */
static float LS2PI  =  0.91893853320467274178;
#define MAXLGM 2.035093e36f
static float PIINV =  0.318309886183790671538;

/* Logarithm of gamma function */


#ifdef ANSIC
float lgam( float xx )
#else
float lgam(xx)
double xx;
#endif
{
float p, q, w, z, x;
float nx, tx;
int i, direction;

sgngamf = 1;

x = xx;
if( x < 0.0f )
	{
	q = -x;
	w = lgam(q); /* note this modifies sgngam! */
	p = floor(q);
	if( p == q )
		goto loverf;
	i = p;
	if( (i & 1) == 0 )
		sgngamf = -1;
	else
		sgngamf = 1;
	z = q - p;
	if( z > 0.5f )
		{
		p += 1.0f;
		z = p - q;
		}
	z = q * sin( PIF * z );
	if( z == 0.0f )
		goto loverf;
	z = -log( PIINV*z ) - w;
	return( z );
	}

if( x < 6.5f )
	{
	direction = 0;
	z = 1.0f;
	tx = x;
	nx = 0.0f;
	if( x >= 1.5f )
		{
		while( tx > 2.5f )
			{
			nx -= 1.0f;
			tx = x + nx;
			z *=tx;
			}
		x += nx - 2.0f;
iv1r5:
		p = x * polevl( x, B, 7 );
		goto cont;
		}
	if( x >= 1.25f )
		{
		z *= x;
		x -= 1.0f; /* x + 1 - 2 */
		direction = 1;
		goto iv1r5;
		}
	if( x >= 0.75f )
		{
		x -= 1.0f;
		p = x * polevl( x, C, 7 );
		q = 0.0f;
		goto contz;
		}
	while( tx < 1.5f )
		{
		if( tx == 0.0f )
			goto loverf;
		z *=tx;
		nx += 1.0f;
		tx = x + nx;
		}
	direction = 1;
	x += nx - 2.0f;
	p = x * polevl( x, B, 7 );

cont:
	if( z < 0.0f )
		{
		sgngamf = -1;
		z = -z;
		}
	else
		{
		sgngamf = 1;
		}
	q = log(z);
	if( direction )
		q = -q;
contz:
	return( p + q );
	}

if( x > MAXLGM )
	{
loverf:
	mtherr( "lgam", OVERFLOW );
	return( sgngamf * MAXNUMF );
	}

/* Note, though an asymptotic formula could be used for x >= 3,
 * there is cancellation error in the following if x < 6.5.  */
q = LS2PI - x;
q += ( x - 0.5f ) * log(x);

if( x <= 1.0e4f )
	{
	z = 1.0f/x;
	p = z * z;
	q += ((    6.789774945028216E-004f * p
		 - 2.769887652139868E-003f ) * p
		+  8.333316229807355E-002f ) * z;
	}
return( q );
}
