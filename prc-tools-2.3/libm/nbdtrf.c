/*							nbdtr.c
 *
 *	Negative binomial distribution
 *
 *
 *
 * SYNOPSIS:
 *
 * int k, n;
 * float p, y, nbdtr();
 *
 * y = nbdtr( k, n, p );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the sum of the terms 0 through k of the negative
 * binomial distribution:
 *
 *   k
 *   --  ( n+j-1 )   n      j
 *   >   (       )  p  (1-p)
 *   --  (   j   )
 *  j=0
 *
 * In a sequence of Bernoulli trials, this is the probability
 * that k or fewer failures precede the nth success.
 *
 * The terms are not computed individually; instead the incomplete
 * beta integral is employed, according to the formula
 *
 * y = nbdtr( k, n, p ) = incbet( n, k+1, p ).
 *
 * The arguments must be positive, with p ranging from 0 to 1.
 *
 *
 *
 * ACCURACY:
 *
 *        Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,100       5000       1.5e-4      1.9e-5
 *
 */
/*							nbdtrc.c
 *
 *	Complemented negative binomial distribution
 *
 *
 *
 * SYNOPSIS:
 *
 * int k, n;
 * float p, y, nbdtrc();
 *
 * y = nbdtrc( k, n, p );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns the sum of the terms k+1 to infinity of the negative
 * binomial distribution:
 *
 *   inf
 *   --  ( n+j-1 )   n      j
 *   >   (       )  p  (1-p)
 *   --  (   j   )
 *  j=k+1
 *
 * The terms are not computed individually; instead the incomplete
 * beta integral is employed, according to the formula
 *
 * y = nbdtrc( k, n, p ) = incbet( k+1, n, 1-p ).
 *
 * The arguments must be positive, with p ranging from 0 to 1.
 *
 *
 *
 * ACCURACY:
 *
 *        Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE       0,100       5000       1.4e-4      2.0e-5
 *
 */

/*
Cephes Math Library Release 2.2:  July, 1992
Copyright 1984, 1987 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

#ifdef ANSIC
float incbet(float, float, float);
#else
float incbet();
#endif


#ifdef ANSIC
float nbdtrc( int k, int n, float pp )
#else
float nbdtrc( k, n, pp )
int k, n;
double pp;
#endif
{
float dk, dn, p;

p = pp;
if( (p < 0.0f) || (p > 1.0f) )
	goto domerr;
if( k < 0 )
	{
domerr:
	mtherr( "nbdtr", DOMAIN );
	return( 0.0f );
	}

dk = k+1;
dn = n;
return( incbet( dk, dn, 1.0f - p ) );
}



#ifdef ANSIC
float nbdtr( int k, int n, float pp )
#else
float nbdtr( k, n, pp )
int k, n;
double pp;
#endif
{
float dk, dn, p;

p = pp;
if( (p < 0.0f) || (p > 1.0f) )
	goto domerr;
if( k < 0 )
	{
domerr:
	mtherr( "nbdtr", DOMAIN );
	return( 0.0f );
	}
dk = k+1;
dn = n;
return( incbet( dn, dk, p ) );
}
