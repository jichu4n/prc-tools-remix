/*							k1.c
 *
 *	Modified Bessel function, third kind, order one
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, k1();
 *
 * y = k1( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Computes the modified Bessel function of the third kind
 * of order one of the argument.
 *
 * The range is partitioned into the two intervals [0,2] and
 * (2, infinity).  Chebyshev polynomial expansions are employed
 * in each interval.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0, 30       30000       4.6e-7      7.6e-8
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * k1 domain          x <= 0          MAXNUM
 *
 */
/*							k1e.c
 *
 *	Modified Bessel function, third kind, order one,
 *	exponentially scaled
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, k1e();
 *
 * y = k1e( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns exponentially scaled modified Bessel function
 * of the third kind of order one of the argument:
 *
 *      k1e(x) = exp(x) * k1(x).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0, 30       30000       4.9e-7      6.7e-8
 * See k1().
 *
 */

/*
Cephes Math Library Release 2.2: June, 1992
Copyright 1984, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

/* Chebyshev coefficients for x(K1(x) - log(x/2) I1(x))
 * in the interval [0,2].
 * 
 * lim(x->0){ x(K1(x) - log(x/2) I1(x)) } = 1.
 */

#define MINNUMF 6.0e-39f
static float A[] =
{
-2.21338763073472585583E-8,
-2.43340614156596823496E-6,
-1.73028895751305206302E-4,
-6.97572385963986435018E-3,
-1.22611180822657148235E-1,
-3.53155960776544875667E-1,
 1.52530022733894777053E0
};




/* Chebyshev coefficients for exp(x) sqrt(x) K1(x)
 * in the interval [2,infinity].
 *
 * lim(x->inf){ exp(x) sqrt(x) K1(x) } = sqrt(pi/2).
 */

static float B[] =
{
 2.01504975519703286596E-9,
-1.03457624656780970260E-8,
 5.74108412545004946722E-8,
-3.50196060308781257119E-7,
 2.40648494783721712015E-6,
-1.93619797416608296024E-5,
 1.95215518471351631108E-4,
-2.85781685962277938680E-3,
 1.03923736576817238437E-1,
 2.72062619048444266945E0
};


 
extern float MAXNUMF;
#ifdef ANSIC
float chbevl(float, float *, int);
float exp(float), i1(float), log(float), sqrt(float);
#else
float chbevl(), exp(), i1(), log(), sqrt();
#endif

#ifdef ANSIC
float k1(float xx)
#else
float k1(xx)
double xx;
#endif
{
float x, y;

x = xx;
if( x <= MINNUMF )
	{
	mtherr( "k1", DOMAIN );
	return( MAXNUMF );
	}

if( x <= 2.0f )
	{
	y = x * x - 2.0f;
	y =  log( 0.5f * x ) * i1(x)  +  chbevl( y, A, 7 ) / x;
	return( y );
	}

return(  exp(-x) * chbevl( 8.0f/x - 2.0f, B, 10 ) / sqrt(x) );

}



#ifdef ANSIC
float k1e( float xx )
#else
float k1e( xx )
double xx;
#endif
{
float x, y;

x = xx;
if( x <= 0.0f )
	{
	mtherr( "k1e", DOMAIN );
	return( MAXNUMF );
	}

if( x <= 2.0f )
	{
	y = x * x - 2.0f;
	y =  log( 0.5f * x ) * i1(x)  +  chbevl( y, A, 7 ) / x;
	return( y * exp(x) );
	}

return(  chbevl( 8.0f/x - 2.0f, B, 10 ) / sqrt(x) );

}
