/*							k0.c
 *
 *	Modified Bessel function, third kind, order zero
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, k0();
 *
 * y = k0( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns modified Bessel function of the third kind
 * of order zero of the argument.
 *
 * The range is partitioned into the two intervals [0,8] and
 * (8, infinity).  Chebyshev polynomial expansions are employed
 * in each interval.
 *
 *
 *
 * ACCURACY:
 *
 * Tested at 2000 random points between 0 and 8.  Peak absolute
 * error (relative when K0 > 1) was 1.46e-14; rms, 4.26e-15.
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0, 30       30000       7.8e-7      8.5e-8
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 *  K0 domain          x <= 0          MAXNUM
 *
 */
/*							k0e()
 *
 *	Modified Bessel function, third kind, order zero,
 *	exponentially scaled
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, k0e();
 *
 * y = k0e( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns exponentially scaled modified Bessel function
 * of the third kind of order zero of the argument.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0, 30       30000       8.1e-7      7.8e-8
 * See k0().
 *
 */

/*
Cephes Math Library Release 2.0:  April, 1987
Copyright 1984, 1987 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

/* Chebyshev coefficients for K0(x) + log(x/2) I0(x)
 * in the interval [0,2].  The odd order coefficients are all
 * zero; only the even order coefficients are listed.
 * 
 * lim(x->0){ K0(x) + log(x/2) I0(x) } = -EUL.
 */

static float A[] =
{
 1.90451637722020886025E-9,
 2.53479107902614945675E-7,
 2.28621210311945178607E-5,
 1.26461541144692592338E-3,
 3.59799365153615016266E-2,
 3.44289899924628486886E-1,
-5.35327393233902768720E-1
};



/* Chebyshev coefficients for exp(x) sqrt(x) K0(x)
 * in the inverted interval [2,infinity].
 * 
 * lim(x->inf){ exp(x) sqrt(x) K0(x) } = sqrt(pi/2).
 */

static float B[] = {
-1.69753450938905987466E-9,
 8.57403401741422608519E-9,
-4.66048989768794782956E-8,
 2.76681363944501510342E-7,
-1.83175552271911948767E-6,
 1.39498137188764993662E-5,
-1.28495495816278026384E-4,
 1.56988388573005337491E-3,
-3.14481013119645005427E-2,
 2.44030308206595545468E0
};

/*							k0.c	*/
 
extern float MAXNUMF;

#ifdef ANSIC
float chbevl(float, float *, int);
float exp(float), i0(float), log(float), sqrt(float);
#else
float chbevl(), exp(), i0(), log(), sqrt();
#endif


#ifdef ANSIC
float k0( float xx )
#else
float k0(xx)
double xx;
#endif
{
float x, y, z;

x = xx;
if( x <= 0.0f )
	{
	mtherr( "k0", DOMAIN );
	return( MAXNUMF );
	}

if( x <= 2.0f )
	{
	y = x * x - 2.0f;
	y = chbevl( y, A, 7 ) - log( 0.5f * x ) * i0(x);
	return( y );
	}
z = 8.0f/x - 2.0f;
y = exp(-x) * chbevl( z, B, 10 ) / sqrt(x);
return(y);
}



#ifdef ANSIC
float k0e( float xx )
#else
float k0e( xx )
double xx;
#endif
{
float x, y;


x = xx;
if( x <= 0.0f )
	{
	mtherr( "k0e", DOMAIN );
	return( MAXNUMF );
	}

if( x <= 2.0f )
	{
	y = x * x - 2.0f;
	y = chbevl( y, A, 7 ) - log( 0.5f * x ) * i0(x);
	return( y * exp(x) );
	}

y = chbevl( 8.0f/x - 2.0f, B, 10 ) / sqrt(x);
return(y);
}
