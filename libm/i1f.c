/*							i1.c
 *
 *	Modified Bessel function of order one
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, i1();
 *
 * y = i1( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns modified Bessel function of order one of the
 * argument.
 *
 * The function is defined as i1(x) = -i j1( ix ).
 *
 * The range is partitioned into the two intervals [0,8] and
 * (8, infinity).  Chebyshev polynomial expansions are employed
 * in each interval.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0, 30       100000      1.5e-6      1.6e-7
 *
 *
 */
/*							i1e.c
 *
 *	Modified Bessel function of order one,
 *	exponentially scaled
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, i1e();
 *
 * y = i1e( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns exponentially scaled modified Bessel function
 * of order one of the argument.
 *
 * The function is defined as i1(x) = -i exp(-|x|) j1( ix ).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0, 30       30000       1.5e-6      1.5e-7
 * See i1().
 *
 */

/*							i1.c 2		*/


/*
Cephes Math Library Release 2.0:  March, 1987
Copyright 1985, 1987 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

/* Chebyshev coefficients for exp(-x) I1(x) / x
 * in the interval [0,8].
 *
 * lim(x->0){ exp(-x) I1(x) / x } = 1/2.
 */

static float A[] =
{
 9.38153738649577178388E-9,
-4.44505912879632808065E-8,
 2.00329475355213526229E-7,
-8.56872026469545474066E-7,
 3.47025130813767847674E-6,
-1.32731636560394358279E-5,
 4.78156510755005422638E-5,
-1.61760815825896745588E-4,
 5.12285956168575772895E-4,
-1.51357245063125314899E-3,
 4.15642294431288815669E-3,
-1.05640848946261981558E-2,
 2.47264490306265168283E-2,
-5.29459812080949914269E-2,
 1.02643658689847095384E-1,
-1.76416518357834055153E-1,
 2.52587186443633654823E-1
};


/* Chebyshev coefficients for exp(-x) sqrt(x) I1(x)
 * in the inverted interval [8,infinity].
 *
 * lim(x->inf){ exp(-x) sqrt(x) I1(x) } = 1/sqrt(2pi).
 */

static float B[] =
{
-3.83538038596423702205E-9,
-2.63146884688951950684E-8,
-2.51223623787020892529E-7,
-3.88256480887769039346E-6,
-1.10588938762623716291E-4,
-9.76109749136146840777E-3,
 7.78576235018280120474E-1
};

/*							i1.c	*/

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float chbevl(float, float *, int);
float exp(float), sqrt(float);
#else
float chbevl(), exp(), sqrt();
#endif


#ifdef ANSIC
float i1(float xx)
#else
float i1(xx)
double xx;
#endif
{ 
float x, y, z;

x = xx;
z = fabsf(x);
if( z <= 8.0f )
	{
	y = 0.5f*z - 2.0f;
	z = chbevl( y, A, 17 ) * z * exp(z);
	}
else
	{
	z = exp(z) * chbevl( 32.0f/z - 2.0f, B, 7 ) / sqrt(z);
	}
if( x < 0.0f )
	z = -z;
return( z );
}

/*							i1e()	*/

#ifdef ANSIC
float i1e( float xx )
#else
float i1e( xx )
double xx;
#endif
{ 
float x, y, z;

x = xx;
z = fabsf(x);
if( z <= 8.0f )
	{
	y = 0.5f*z - 2.0f;
	z = chbevl( y, A, 17 ) * z;
	}
else
	{
	z = chbevl( 32.0f/z - 2.0f, B, 7 ) / sqrt(z);
	}
if( x < 0.0f )
	z = -z;
return( z );
}
