/*							chdtr.c
 *
 *	Chi-square distribution
 *
 *
 *
 * SYNOPSIS:
 *
 * float df, x, y, chdtr();
 *
 * y = chdtr( df, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the area under the left hand tail (from 0 to x)
 * of the Chi square probability density function with
 * v degrees of freedom.
 *
 *
 *                                  inf.
 *                                    -
 *                        1          | |  v/2-1  -t/2
 *  P( x | v )   =   -----------     |   t      e     dt
 *                    v/2  -       | |
 *                   2    | (v/2)   -
 *                                   x
 *
 * where x is the Chi-square variable.
 *
 * The incomplete gamma integral is used, according to the
 * formula
 *
 *	y = chdtr( v, x ) = igam( v/2.0, x/2.0 ).
 *
 *
 * The arguments must both be positive.
 *
 *
 *
 * ACCURACY:
 *
 *        Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,100       5000       3.2e-5      5.0e-6
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * chdtr domain  x < 0 or v < 1        0.0
 */
/*							chdtrc()
 *
 *	Complemented Chi-square distribution
 *
 *
 *
 * SYNOPSIS:
 *
 * float v, x, y, chdtrc();
 *
 * y = chdtrc( v, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the area under the right hand tail (from x to
 * infinity) of the Chi square probability density function
 * with v degrees of freedom:
 *
 *
 *                                  inf.
 *                                    -
 *                        1          | |  v/2-1  -t/2
 *  P( x | v )   =   -----------     |   t      e     dt
 *                    v/2  -       | |
 *                   2    | (v/2)   -
 *                                   x
 *
 * where x is the Chi-square variable.
 *
 * The incomplete gamma integral is used, according to the
 * formula
 *
 *	y = chdtr( v, x ) = igamc( v/2.0, x/2.0 ).
 *
 *
 * The arguments must both be positive.
 *
 *
 *
 * ACCURACY:
 *
 *        Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,100       5000       2.7e-5      3.2e-6
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * chdtrc domain  x < 0 or v < 1        0.0
 */
/*							chdtri()
 *
 *	Inverse of complemented Chi-square distribution
 *
 *
 *
 * SYNOPSIS:
 *
 * float df, x, y, chdtri();
 *
 * x = chdtri( df, y );
 *
 *
 *
 *
 * DESCRIPTION:
 *
 * Finds the Chi-square argument x such that the integral
 * from x to infinity of the Chi-square density is equal
 * to the given cumulative probability y.
 *
 * This is accomplished using the inverse gamma integral
 * function and the relation
 *
 *    x/2 = igami( df/2, y );
 *
 *
 *
 *
 * ACCURACY:
 *
 *        Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,100       10000      2.2e-5      8.5e-7
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * chdtri domain   y < 0 or y > 1        0.0
 *                     v < 1
 *
 */

/*								chdtr() */


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

#ifdef ANSIC
float igamc(float, float), igam(float, float), igami(float, float);
#else
float igamc(), igam(), igami();
#endif

#ifdef ANSIC
float chdtrc(float dff, float xx)
#else
float chdtrc(dff,xx)
double dff, xx;
#endif
{
float df, x;

df = dff;
x = xx;

if( (x < 0.0f) || (df < 1.0f) )
	{
	mtherr( "chdtrc", DOMAIN );
	return(0.0f);
	}
return( igamc( 0.5f*df, 0.5f*x ) );
}


#ifdef ANSIC
float chdtr(float dff, float xx)
#else
float chdtr(dff,xx)
double dff, xx;
#endif
{
float df, x;

df = dff;
x = xx;
if( (x < 0.0f) || (df < 1.0f) )
	{
	mtherr( "chdtr", DOMAIN );
	return(0.0f);
	}
return( igam( 0.5f*df, 0.5f*x ) );
}


#ifdef ANSIC
float chdtri( float dff, float yy )
#else
float chdtri( dff, yy )
double dff, yy;
#endif
{
float y, df, x;

y = yy;
df = dff;
if( (y < 0.0f) || (y > 1.0f) || (df < 1.0f) )
	{
	mtherr( "chdtri", DOMAIN );
	return(0.0f);
	}

x = igami( 0.5f * df, y );
return( 2.0f * x );
}
