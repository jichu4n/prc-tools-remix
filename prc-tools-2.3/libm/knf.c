/*							kn.c
 *
 *	Modified Bessel function, third kind, integer order
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, kn();
 * int n;
 *
 * y = kn( n, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns modified Bessel function of the third kind
 * of order n of the argument.
 *
 * The range is partitioned into the two intervals [0,9.55] and
 * (9.55, infinity).  An ascending power series is used in the
 * low range, and an asymptotic expansion in the high range.
 *
 *
 *
 * ACCURACY:
 *
 *          Absolute error, relative when function > 1:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0,30        10000       2.0e-4      3.8e-6
 *
 *  Error is high only near the crossover point x = 9.55
 * between the two expansions used.
 */


/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1988, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140

*/


/*
Algorithm for Kn.
                       n-1 
                   -n   -  (n-k-1)!    2   k
K (x)  =  0.5 (x/2)     >  -------- (-x /4)
 n                      -     k!
                       k=0

                    inf.                                   2   k
       n         n   -                                   (x /4)
 + (-1)  0.5(x/2)    >  {p(k+1) + p(n+k+1) - 2log(x/2)} ---------
                     -                                  k! (n+k)!
                    k=0

where  p(m) is the psi function: p(1) = -EUL and

                      m-1
                       -
      p(m)  =  -EUL +  >  1/k
                       -
                      k=1

For large x,
                                         2        2     2
                                      u-1     (u-1 )(u-3 )
K (z)  =  sqrt(pi/2z) exp(-z) { 1 + ------- + ------------ + ...}
 v                                        1            2
                                    1! (8z)     2! (8z)
asymptotically, where

           2
    u = 4 v .

*/

#include "mconf.h"

#define EUL 5.772156649015328606065e-1f
#define MAXFAC 31
extern float MACHEPF, MAXNUMF, MAXLOGF, PIF;

#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float exp(float), log(float), sqrt(float);

float kn( int nnn, float xx )
#else
float exp(), log(), sqrt();

float kn( nnn, xx )
int nnn;
double xx;
#endif
{
float x, k, kf, nk1, nkf, zn, t, s, z0, z;
float ans, fn, pn, pk, zmn, tlg, tox;
int i, n, nn;

nn = nnn;
x = xx;
if( nn < 0 )
	n = -nn;
else
	n = nn;

if( n > MAXFAC )
	{
overf:
	mtherr( "kn", OVERFLOW );
	return( MAXNUMF );
	}

if( x <= 0.0f )
	{
	if( x < 0.0f )
		mtherr( "kn", DOMAIN );
	else
		mtherr( "kn", SING );
	return( MAXNUMF );
	}


if( x > 9.55f )
	goto asymp;

ans = 0.0f;
z0 = 0.25f * x * x;
fn = 1.0f;
pn = 0.0f;
zmn = 1.0f;
tox = 2.0f/x;

if( n > 0 )
	{
	/* compute factorial of n and psi(n) */
	pn = -EUL;
	k = 1.0f;
	for( i=1; i<n; i++ )
		{
		pn += 1.0f/k;
		k += 1.0f;
		fn *= k;
		}

	zmn = tox;

	if( n == 1 )
		{
		ans = 1.0f/x;
		}
	else
		{
		nk1 = fn/n;
		kf = 1.0f;
		s = nk1;
		z = -z0;
		zn = 1.0f;
		for( i=1; i<n; i++ )
			{
			nk1 = nk1/(n-i);
			kf = kf * i;
			zn *= z;
			t = nk1 * zn / kf;
			s += t;   
			if( (MAXNUMF - fabsf(t)) < fabsf(s) )
				goto overf;
			if( (tox > 1.0f) && ((MAXNUMF/tox) < zmn) )
				goto overf;
			zmn *= tox;
			}
		s *= 0.5f;
		t = fabsf(s);
		if( (zmn > 1.0f) && ((MAXNUMF/zmn) < t) )
			goto overf;
		if( (t > 1.0f) && ((MAXNUMF/t) < zmn) )
			goto overf;
		ans = s * zmn;
		}
	}


tlg = 2.0f * log( 0.5f * x );
pk = -EUL;
if( n == 0 )
	{
	pn = pk;
	t = 1.0f;
	}
else
	{
	pn = pn + 1.0f/n;
	t = 1.0f/fn;
	}
s = (pk+pn-tlg)*t;
k = 1.0f;
do
	{
	t *= z0 / (k * (k+n));
	pk += 1.0f/k;
	pn += 1.0f/(k+n);
	s += (pk+pn-tlg)*t;
	k += 1.0f;
	}
while( fabsf(t/s) > MACHEPF );

s = 0.5f * s / zmn;
if( n & 1 )
	s = -s;
ans += s;

return(ans);



/* Asymptotic expansion for Kn(x) */
/* Converges to 1.4e-17 for x > 18.4 */

asymp:

if( x > MAXLOGF )
	{
	mtherr( "kn", UNDERFLOW );
	return(0.0f);
	}
k = n;
pn = 4.0f * k * k;
pk = 1.0f;
z0 = 8.0f * x;
fn = 1.0f;
t = 1.0f;
s = t;
nkf = MAXNUMF;
i = 0;
do
	{
	z = pn - pk * pk;
	t = t * z /(fn * z0);
	nk1 = fabsf(t);
	if( (i >= n) && (nk1 > nkf) )
		{
		goto adone;
		}
	nkf = nk1;
	s += t;
	fn += 1.0f;
	pk += 2.0f;
	i += 1;
	}
while( fabsf(t/s) > MACHEPF );

adone:
ans = exp(-x) * sqrt( PIF/(2.0f*x) ) * s;
return(ans);
}
