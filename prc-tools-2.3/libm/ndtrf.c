/*							ndtr.c
 *
 *	Normal distribution function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, ndtr();
 *
 * y = ndtr( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the area under the Gaussian probability density
 * function, integrated from minus infinity to x:
 *
 *                            x
 *                             -
 *                   1        | |          2
 *    ndtr(x)  = ---------    |    exp( - t /2 ) dt
 *               sqrt(2pi)  | |
 *                           -
 *                          -inf.
 *
 *             =  ( 1 + erf(z) ) / 2
 *             =  erfc(z) / 2
 *
 * where z = x/sqrt(2). Computation is via the functions
 * erf and erfc.
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE     -13,0        50000       1.5e-5      2.6e-6
 *
 *
 * ERROR MESSAGES:
 *
 * See erfc().
 *
 */
/*							erf.c
 *
 *	Error function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, erf();
 *
 * y = erf( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * The integral is
 *
 *                           x 
 *                            -
 *                 2         | |          2
 *   erf(x)  =  --------     |    exp( - t  ) dt.
 *              sqrt(pi)   | |
 *                          -
 *                           0
 *
 * The magnitude of x is limited to 9.231948545 for DEC
 * arithmetic; 1 or -1 is returned outside this range.
 *
 * For 0 <= |x| < 1, erf(x) = x * P(x**2); otherwise
 * erf(x) = 1 - erfc(x).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -9.3,9.3    50000       1.7e-7      2.8e-8
 *
 */
/*							erfc.c
 *
 *	Complementary error function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, erfc();
 *
 * y = erfc( x );
 *
 *
 *
 * DESCRIPTION:
 *
 *
 *  1 - erf(x) =
 *
 *                           inf. 
 *                             -
 *                  2         | |          2
 *   erfc(x)  =  --------     |    exp( - t  ) dt
 *               sqrt(pi)   | |
 *                           -
 *                            x
 *
 *
 * For small x, erfc(x) = 1 - erf(x); otherwise polynomial
 * approximations 1/x P(1/x**2) are computed.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -9.3,9.3    50000       3.9e-6      7.2e-7
 *
 *
 * ERROR MESSAGES:
 *
 *   message           condition              value returned
 * erfc underflow    x**2 > MAXLOGF              0.0
 *
 *
 */


/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1988 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "mconf.h"

extern float MAXLOGF, SQRTHF;


/* erfc(x) = exp(-x^2) P(1/x), 1 < x < 2 */
static float P[] = {
 2.326819970068386E-002,
-1.387039388740657E-001,
 3.687424674597105E-001,
-5.824733027278666E-001,
 6.210004621745983E-001,
-4.944515323274145E-001,
 3.404879937665872E-001,
-2.741127028184656E-001,
 5.638259427386472E-001
};

/* erfc(x) = exp(-x^2) 1/x P(1/x^2), 2 < x < 14 */
static float R[] = {
-1.047766399936249E+001,
 1.297719955372516E+001,
-7.495518717768503E+000,
 2.921019019210786E+000,
-1.015265279202700E+000,
 4.218463358204948E-001,
-2.820767439740514E-001,
 5.641895067754075E-001
};

/* erf(x) = x P(x^2), 0 < x < 1 */
static float T[] = {
 7.853861353153693E-005,
-8.010193625184903E-004,
 5.188327685732524E-003,
-2.685381193529856E-002,
 1.128358514861418E-001,
-3.761262582423300E-001,
 1.128379165726710E+000
};

/*#define UTHRESH 37.519379347*/

#define UTHRESH 14.0f

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float polevl(float, float *, int);
float exp(float), log(float), erf(float), erfc(float);
#else
float polevl(), exp(), log(), erf(), erfc();
#endif



#ifdef ANSIC
float ndtr(float aa)
#else
float ndtr(aa)
double aa;
#endif
{
float x, y, z;

x = aa;
x *= SQRTHF;
z = fabsf(x);

if( z < SQRTHF )
	y = 0.5f + 0.5f * erf(x);
else
	{
	y = 0.5f * erfc(z);

	if( x > 0 )
		y = 1.0f - y;
	}

return(y);
}


#ifdef ANSIC
float erfc(float aa)
#else
float erfc(aa)
double aa;
#endif
{
float a, p,q,x,y,z;


a = aa;
x = fabsf(a);

if( x < 1.0f )
	return( 1.0f - erf(a) );

z = -a * a;

if( z < -MAXLOGF )
	{
under:
	mtherr( "erfc", UNDERFLOW );
	if( a < 0 )
		return( 2.0f );
	else
		return( 0.0f );
	}

z = exp(z);
q = 1.0f/x;
y = q * q;
if( x < 2.0f )
	{
	p = polevl( y, P, 8 );
	}
else
	{
	p = polevl( y, R, 7 );
	}

y = z * q * p;

if( a < 0 )
	y = 2.0f - y;

if( y == 0.0f )
	goto under;

return(y);
}


#ifdef ANSIC
float erf(float xx)
#else
float erf(xx)
double xx;
#endif
{
float x, y, z;

x = xx;
if( fabsf(x) > 1.0f )
	return( 1.0f - erfc(x) );

z = x * x;
y = x * polevl( z, T, 6 );
return( y );

}
