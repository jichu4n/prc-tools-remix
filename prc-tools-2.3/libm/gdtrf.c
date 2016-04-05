/*							gdtr.c
 *
 *	Gamma distribution function
 *
 *
 *
 * SYNOPSIS:
 *
 * float a, b, x, y, gdtr();
 *
 * y = gdtr( a, b, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the integral from zero to x of the gamma probability
 * density function:
 *
 *
 *                x
 *        b       -
 *       a       | |   b-1  -at
 * y =  -----    |    t    e    dt
 *       -     | |
 *      | (b)   -
 *               0
 *
 *  The incomplete gamma integral is used, according to the
 * relation
 *
 * y = igam( b, ax ).
 *
 *
 * ACCURACY:
 *
 *        Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,100       5000       5.8e-5      3.0e-6
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * gdtr domain        x < 0            0.0
 *
 */
/*							gdtrc.c
 *
 *	Complemented gamma distribution function
 *
 *
 *
 * SYNOPSIS:
 *
 * float a, b, x, y, gdtrc();
 *
 * y = gdtrc( a, b, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the integral from x to infinity of the gamma
 * probability density function:
 *
 *
 *               inf.
 *        b       -
 *       a       | |   b-1  -at
 * y =  -----    |    t    e    dt
 *       -     | |
 *      | (b)   -
 *               x
 *
 *  The incomplete gamma integral is used, according to the
 * relation
 *
 * y = igamc( b, ax ).
 *
 *
 * ACCURACY:
 *
 *        Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,100       5000       9.1e-5      1.5e-5
 *
 * ERROR MESSAGES:
 *
 *   message         condition      value returned
 * gdtrc domain        x < 0            0.0
 *
 */

/*							gdtr()  */


/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"
#ifdef ANSIC
float igam(float, float), igamc(float, float);
#else
float igam(), igamc();
#endif



#ifdef ANSIC
float gdtr( float aa, float bb, float xx )
#else
float gdtr( aa, bb, xx )
double aa, bb, xx;
#endif
{
float a, b, x;

a = aa;
b = bb;
x = xx;


if( x < 0.0f )
	{
	mtherr( "gdtr", DOMAIN );
	return( 0.0f );
	}
return(  igam( b, a * x )  );
}



#ifdef ANSIC
float gdtrc( float aa, float bb, float xx )
#else
float gdtrc( aa, bb, xx )
double aa, bb, xx;
#endif
{
float a, b, x;

a = aa;
b = bb;
x = xx;
if( x < 0.0f )
	{
	mtherr( "gdtrc", DOMAIN );
	return( 0.0f );
	}
return(  igamc( b, a * x )  );
}
