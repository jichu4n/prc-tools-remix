/*							jv.c
 *
 *	Bessel function of noninteger order
 *
 *
 *
 * SYNOPSIS:
 *
 * float v, x, y, jv();
 *
 * y = jv( v, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns Bessel function of order v of the argument,
 * where v is real.  Negative x is allowed if v is an integer.
 *
 * Several expansions are included: the ascending power
 * series, the Hankel expansion, and two transitional
 * expansions for large v.  If v is not too large, it
 * is reduced by recurrence to a region of best accuracy.
 *
 * The single precision routine accepts negative v, but with
 * reduced accuracy.
 *
 *
 *
 * ACCURACY:
 * Results for integer v are indicated by *.
 * Error criterion is absolute, except relative when |jv()| > 1.
 *
 * arithmetic     domain      # trials      peak         rms
 *                v      x
 *    IEEE       0,125  0,125   30000      2.0e-6      2.0e-7
 *    IEEE     -17,0    0,125   30000      1.1e-5      4.0e-7
 *    IEEE    -100,0    0,125    3000      1.5e-4      7.8e-6
 */


/*
Cephes Math Library Release 2.2: June, 1992
Copyright 1984, 1987, 1989, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/


#include "mconf.h"
#define DEBUG 0

extern float MAXNUMF, MACHEPF, MINLOGF, MAXLOGF, PIF;
extern int sgngamf;

/* BIG = 1/MACHEPF */
#define BIG   16777216.

#ifdef ANSIC
float floor(float), j0(float), j1(float);
static float jnxf(float, float);
static float jvsf(float, float);
static float hankelf(float, float);
static float jntf(float, float);
static float recurf( float *, float, float * );
float sqrt(float), sin(float), cos(float);
float lgam(float), exp(float), log(float), pow(float, float);
float gamma(float), cbrt(float), acos(float);
int airy(float, float *, float *, float *, float *);
float polevl(float, float *, int);
#else
float floor(), j0(), j1();
float sqrt(), sin(), cos();
float lgam(), exp(), log(), pow(), gamma();
float cbrt(), polevl(), acos();
void airy();
static float recurf(), jvsf(), hankelf(), jnxf(), jntf(), jvsf();
#endif


#define fabsf(x) ( (x) < 0 ? -(x) : (x) )

#ifdef ANSIC
float jv( float nn, float xx )
#else
float jv( nn, xx )
double nn, xx;
#endif
{
float n, x, k, q, t, y, an, sign;
int i, nint;

n = nn;
x = xx;
nint = 0;	/* Flag for integer n */
sign = 1.0f;	/* Flag for sign inversion */
an = fabsf( n );
y = floor( an );
if( y == an )
	{
	nint = 1;
	i = an - 16384.0f * floor( an/16384.0f );
	if( n < 0.0f )
		{
		if( i & 1 )
			sign = -sign;
		n = an;
		}
	if( x < 0.0f )
		{
		if( i & 1 )
			sign = -sign;
		x = -x;
		}
	if( n == 0.0f )
		return( j0(x) );
	if( n == 1.0f )
		return( sign * j1(x) );
	}

if( (x < 0.0f) && (y != an) )
	{
	mtherr( "jv", DOMAIN );
	y = 0.0f;
	goto done;
	}

y = fabsf(x);

if( y < MACHEPF )
	goto underf;

/* Easy cases - x small compared to n */
t = 3.6f * sqrt(an);
if( y < t )
	return( sign * jvsf(n,x) );

/* x large compared to n */
k = 3.6f * sqrt(y);
if( (an < k) && (y > 6.0f) )
	return( sign * hankelf(n,x) );

if( (n > -100) && (n < 14.0f) )
	{
/* Note: if x is too large, the continued
 * fraction will fail; but then the
 * Hankel expansion can be used.
 */
	if( nint != 0 )
		{
		k = 0.0f;
		q = recurf( &n, x, &k );
		if( k == 0.0f )
			{
			y = j0(x)/q;
			goto done;
			}
		if( k == 1.0f )
			{
			y = j1(x)/q;
			goto done;
			}
		}

	if( n >= 0.0f )
		{
/* Recur backwards from a larger value of n
 */
		if( y > 1.3f * an )
			goto recurdwn;
		if( an > 1.3f * y )
			goto recurdwn;
		k = n;
		y = 2.0f*(y+an+1.0f);
		if( (y - n) > 33.0f )
			y = n + 33.0f;
		y = n + floor(y-n);
		q = recurf( &y, x, &k );
		y = jvsf(y,x) * q;
		goto done;
		}
recurdwn:
	if( an > (k + 3.0f) )
		{
/* Recur backwards from n to k
 */
		if( n < 0.0f )
			k = -k;
		q = n - floor(n);
		k = floor(k) + q;
		if( n > 0.0f )
			q = recurf( &n, x, &k );
		else
			{
			t = k;
			k = n;
			q = recurf( &t, x, &k );
			k = t;
			}
		if( q == 0.0f )
			{
underf:
			y = 0.0f;
			goto done;
			}
		}
	else
		{
		k = n;
		q = 1.0f;
		}

/* boundary between convergence of
 * power series and Hankel expansion
 */
 	t = fabsf(k);
	if( t < 26.0f )
		t = (0.0083f*t + 0.09f)*t + 12.9f;
	else
		t = 0.9f * t;

	if( y > t ) /* y = |x| */
		y = hankelf(k,x);
	else
		y = jvsf(k,x);
#if DEBUG
printf( "y = %.16e, q = %.16e\n", y, q );
#endif
	if( n > 0.0f )
		y /= q;
	else
		y *= q;
	}

else
	{
/* For large positive n, use the uniform expansion
 * or the transitional expansion.
 * But if x is of the order of n**2,
 * these may blow up, whereas the
 * Hankel expansion will then work.
 */
	if( n < 0.0f )
		{
		mtherr( "jv", TLOSS );
		y = 0.0f;
		goto done;
		}
	t = y/an;
	t /= an;
	if( t > 0.3f )
		y = hankelf(n,x);
	else
		y = jnxf(n,x);
	}

done:	return( sign * y);
}

/* Reduce the order by backward recurrence.
 * AMS55 #9.1.27 and 9.1.73.
 */

#ifdef ANSIC
static float recurf( float *n, float xx, float *newn )
#else
static float recurf( n, xx, newn )
float *n;
double xx;
float *newn;
#endif
{
float x, pkm2, pkm1, pk, pkp1, qkm2, qkm1;
float k, ans, qk, xk, yk, r, t, kf, xinv;
static float big = BIG;
int nflag, ctr;

x = xx;
/* continued fraction for Jn(x)/Jn-1(x)  */
if( *n < 0.0f )
	nflag = 1;
else
	nflag = 0;

fstart:

#if DEBUG
printf( "n = %.6e, newn = %.6e, cfrac = ", *n, *newn );
#endif

pkm2 = 0.0f;
qkm2 = 1.0f;
pkm1 = x;
qkm1 = *n + *n;
xk = -x * x;
yk = qkm1;
ans = 1.0f;
ctr = 0;
do
	{
	yk += 2.0f;
	pk = pkm1 * yk +  pkm2 * xk;
	qk = qkm1 * yk +  qkm2 * xk;
	pkm2 = pkm1;
	pkm1 = pk;
	qkm2 = qkm1;
	qkm1 = qk;
	if( qk != 0 )
		r = pk/qk;
	else
		r = 0.0f;
	if( r != 0 )
		{
		t = fabsf( (ans - r)/r );
		ans = r;
		}
	else
		t = 1.0f;

	if( t < MACHEPF )
		goto done;

	if( fabsf(pk) > big )
		{
		pkm2 *= MACHEPF;
		pkm1 *= MACHEPF;
		qkm2 *= MACHEPF;
		qkm1 *= MACHEPF;
		}
	}
while( t > MACHEPF );

done:

#if DEBUG
printf( "%.6e\n", ans );
#endif

/* Change n to n-1 if n < 0 and the continued fraction is small
 */
if( nflag > 0 )
	{
	if( fabsf(ans) < 0.125f )
		{
		nflag = -1;
		*n = *n - 1.0f;
		goto fstart;
		}
	}


kf = *newn;

/* backward recurrence
 *              2k
 *  J   (x)  =  --- J (x)  -  J   (x)
 *   k-1         x   k         k+1
 */

pk = 1.0f;
pkm1 = 1.0f/ans;
k = *n - 1.0f;
r = 2 * k;
xinv = 1.0f/x;
do
	{
	pkm2 = (pkm1 * r  -  pk * x) * xinv;
	pkp1 = pk;
	pk = pkm1;
	pkm1 = pkm2;
	r -= 2.0f;
#if 0
	t = fabsf(pkp1) + fabsf(pk);
	if( (k > (kf + 2.5f)) && (fabsf(pkm1) < 0.25f*t) )
		{
		k -= 1.0f;
		t = x*x;
		pkm2 = ( (r*(r+2.0f)-t)*pk - r*x*pkp1 )/t;
		pkp1 = pk;
		pk = pkm1;
		pkm1 = pkm2;
		r -= 2.0f;
		}
#endif
	k -= 1.0f;
	}
while( k > (kf + 0.5f) );

#if 0
/* Take the larger of the last two iterates
 * on the theory that it may have less cancellation error.
 */
if( (kf >= 0.0f) && (fabsf(pk) > fabsf(pkm1)) )
	{
	k += 1.0f;
	pkm2 = pk;
	}
#endif

*newn = k;
#if DEBUG
printf( "newn %.6e\n", k );
#endif
return( pkm2 );
}



/* Ascending power series for Jv(x).
 * AMS55 #9.1.10.
 */

#ifdef ANSIC
static float jvsf( float nn, float xx )
#else
static float jvsf( nn, xx )
double nn, xx;
#endif
{
float n, x, t, u, y, z, k, ay;

#if DEBUG
printf( "jvsf: " );
#endif
n = nn;
x = xx;
z = -0.25f * x * x;
u = 1.0f;
y = u;
k = 1.0f;
t = 1.0f;

while( t > MACHEPF )
	{
	u *= z / (k * (n+k));
	y += u;
	k += 1.0f;
	t = fabsf(u);
	if( (ay = fabsf(y)) > 1.0f )
		t /= ay;
	}

if( x < 0.0f )
	{
	y = y * pow( 0.5f * x, n ) / gamma( n + 1.0f );
	}
else
	{
	t = n * log(0.5f*x) - lgam(n + 1.0f);
	if( t < -MAXLOGF )
		{
		return( 0.0f );
		}
	if( t > MAXLOGF )
		{
		t = log(y) + t;
		if( t > MAXLOGF )
			{
			mtherr( "jv", OVERFLOW );
			return( MAXNUMF );
			}
		else
			{
			y = sgngamf * exp(t);
			return(y);
			}
		}
	y = sgngamf * y * exp( t );
	}
#if DEBUG
printf( "y = %.8e\n", y );
#endif
return(y);
}

/* Hankel's asymptotic expansion
 * for large x.
 * AMS55 #9.2.5.
 */
#ifdef ANSIC
static float hankelf( float nn, float xx )
#else
static float hankelf( nn, xx )
double nn, xx;
#endif
{
float n, x, t, u, z, k, sign, conv;
float p, q, j, m, pp, qq;
int flag;

#if DEBUG
printf( "hankelf: " );
#endif
n = nn;
x = xx;
m = 4.0f*n*n;
j = 1.0f;
z = 8.0f * x;
k = 1.0f;
p = 1.0f;
u = (m - 1.0f)/z;
q = u;
sign = 1.0f;
conv = 1.0f;
flag = 0;
t = 1.0f;
pp = 1.0e38f;
qq = 1.0e38f;

while( t > MACHEPF )
	{
	k += 2.0f;
	j += 1.0f;
	sign = -sign;
	u *= (m - k * k)/(j * z);
	p += sign * u;
	k += 2.0f;
	j += 1.0f;
	u *= (m - k * k)/(j * z);
	q += sign * u;
	t = fabsf(u/p);
	if( t < conv )
		{
		conv = t;
		qq = q;
		pp = p;
		flag = 1;
		}
/* stop if the terms start getting larger */
	if( (flag != 0) && (t > conv) )
		{
#if DEBUG
		printf( "Hankel: convergence to %.4E\n", conv );
#endif
		goto hank1;
		}
	}	

hank1:
u = x - (0.5f*n + 0.25f) * PIF;
t = sqrt( 2.0f/(PIF*x) ) * ( pp * cos(u) - qq * sin(u) );
return( t );
}


/* Asymptotic expansion for large n.
 * AMS55 #9.3.35.
 */

static float lambda[] = {
  1.0f,
  1.041666666666666666666667E-1,
  8.355034722222222222222222E-2,
  1.282265745563271604938272E-1,
  2.918490264641404642489712E-1,
  8.816272674437576524187671E-1,
  3.321408281862767544702647E+0,
  1.499576298686255465867237E+1,
  7.892301301158651813848139E+1,
  4.744515388682643231611949E+2,
  3.207490090890661934704328E+3
};
static float mu[] = {
  1.0,
 -1.458333333333333333333333E-1,
 -9.874131944444444444444444E-2,
 -1.433120539158950617283951E-1,
 -3.172272026784135480967078E-1,
 -9.424291479571202491373028E-1,
 -3.511203040826354261542798E+0,
 -1.572726362036804512982712E+1,
 -8.228143909718594444224656E+1,
 -4.923553705236705240352022E+2,
 -3.316218568547972508762102E+3
};
static float P1[] = {
 -2.083333333333333333333333E-1,
  1.250000000000000000000000E-1
};
static float P2[] = {
  3.342013888888888888888889E-1,
 -4.010416666666666666666667E-1,
  7.031250000000000000000000E-2
};
static float P3[] = {
 -1.025812596450617283950617E+0,
  1.846462673611111111111111E+0,
 -8.912109375000000000000000E-1,
  7.324218750000000000000000E-2
};
static float P4[] = {
  4.669584423426247427983539E+0,
 -1.120700261622299382716049E+1,
  8.789123535156250000000000E+0,
 -2.364086914062500000000000E+0,
  1.121520996093750000000000E-1
};
static float P5[] = {
 -2.8212072558200244877E1,
  8.4636217674600734632E1,
 -9.1818241543240017361E1,
  4.2534998745388454861E1,
 -7.3687943594796316964E0,
  2.27108001708984375E-1
};
static float P6[] = {
  2.1257013003921712286E2,
 -7.6525246814118164230E2,
  1.0599904525279998779E3,
 -6.9957962737613254123E2,
  2.1819051174421159048E2,
 -2.6491430486951555525E1,
  5.7250142097473144531E-1
};
static float P7[] = {
 -1.9194576623184069963E3,
  8.0617221817373093845E3,
 -1.3586550006434137439E4,
  1.1655393336864533248E4,
 -5.3056469786134031084E3,
  1.2009029132163524628E3,
 -1.0809091978839465550E2,
  1.7277275025844573975E0
};


#ifdef ANSIC
static float jnxf( float nn, float xx )
#else
static float jnxf( nn, xx )
double nn, xx;
#endif
{
float n, x, zeta, sqz, zz, zp, np;
float cbn, n23, t, z, sz;
float pp, qq, z32i, zzi;
float ak, bk, akl, bkl;
int sign, doa, dob, nflg, k, s, tk, tkp1, m;
static float u[8];
static float ai, aip, bi, bip;

n = nn;
x = xx;
/* Test for x very close to n.
 * Use expansion for transition region if so.
 */
cbn = cbrt(n);
z = (x - n)/cbn;
if( (fabsf(z) <= 0.7f) || (n < 0.0f) )
	return( jntf(n,x) );
z = x/n;
zz = 1.0f - z*z;
if( zz == 0.0f )
	return(0.0f);

if( zz > 0.0f )
	{
	sz = sqrt( zz );
	t = 1.5f * (log( (1.0f+sz)/z ) - sz );	/* zeta ** 3/2		*/
	zeta = cbrt( t * t );
	nflg = 1;
	}
else
	{
	sz = sqrt(-zz);
	t = 1.5f * (sz - acos(1.0f/z));
	zeta = -cbrt( t * t );
	nflg = -1;
	}
z32i = fabsf(1.0f/t);
sqz = cbrt(t);

/* Airy function */
n23 = cbrt( n * n );
t = n23 * zeta;

#if DEBUG
printf("zeta %.5E, Airyf(%.5E)\n", zeta, t );
#endif
airy( t, &ai, &aip, &bi, &bip );

/* polynomials in expansion */
u[0] = 1.0f;
zzi = 1.0f/zz;
u[1] = polevl( zzi, P1, 1 )/sz;
u[2] = polevl( zzi, P2, 2 )/zz;
u[3] = polevl( zzi, P3, 3 )/(sz*zz);
pp = zz*zz;
u[4] = polevl( zzi, P4, 4 )/pp;
u[5] = polevl( zzi, P5, 5 )/(pp*sz);
pp *= zz;
u[6] = polevl( zzi, P6, 6 )/pp;
u[7] = polevl( zzi, P7, 7 )/(pp*sz);

#if DEBUG
for( k=0; k<=7; k++ )
	printf( "u[%d] = %.5E\n", k, u[k] );
#endif

pp = 0.0f;
qq = 0.0f;
np = 1.0f;
/* flags to stop when terms get larger */
doa = 1;
dob = 1;
akl = MAXNUMF;
bkl = MAXNUMF;

for( k=0; k<=3; k++ )
	{
	tk = 2 * k;
	tkp1 = tk + 1;
	zp = 1.0f;
	ak = 0.0f;
	bk = 0.0f;
	for( s=0; s<=tk; s++ )
		{
		if( doa )
			{
			if( (s & 3) > 1 )
				sign = nflg;
			else
				sign = 1;
			ak += sign * mu[s] * zp * u[tk-s];
			}

		if( dob )
			{
			m = tkp1 - s;
			if( ((m+1) & 3) > 1 )
				sign = nflg;
			else
				sign = 1;
			bk += sign * lambda[s] * zp * u[m];
			}
		zp *= z32i;
		}

	if( doa )
		{
		ak *= np;
		t = fabsf(ak);
		if( t < akl )
			{
			akl = t;
			pp += ak;
			}
		else
			doa = 0;
		}

	if( dob )
		{
		bk += lambda[tkp1] * zp * u[0];
		bk *= -np/sqz;
		t = fabsf(bk);
		if( t < bkl )
			{
			bkl = t;
			qq += bk;
			}
		else
			dob = 0;
		}
#if DEBUG
	printf("a[%d] %.5E, b[%d] %.5E\n", k, ak, k, bk );
#endif
	if( np < MACHEPF )
		break;
	np /= n*n;
	}

/* normalizing factor ( 4*zeta/(1 - z**2) )**1/4	*/
t = 4.0f * zeta/zz;
t = sqrt( sqrt(t) );

t *= ai*pp/cbrt(n)  +  aip*qq/(n23*n);
return(t);
}

/* Asymptotic expansion for transition region,
 * n large and x close to n.
 * AMS55 #9.3.23.
 */

static float PF2[] = {
 -9.0000000000000000000e-2,
  8.5714285714285714286e-2
};
static float PF3[] = {
  1.3671428571428571429e-1,
 -5.4920634920634920635e-2,
 -4.4444444444444444444e-3
};
static float PF4[] = {
  1.3500000000000000000e-3,
 -1.6036054421768707483e-1,
  4.2590187590187590188e-2,
  2.7330447330447330447e-3
};
static float PG1[] = {
 -2.4285714285714285714e-1,
  1.4285714285714285714e-2
};
static float PG2[] = {
 -9.0000000000000000000e-3,
  1.9396825396825396825e-1,
 -1.1746031746031746032e-2
};
static float PG3[] = {
  1.9607142857142857143e-2,
 -1.5983694083694083694e-1,
  6.3838383838383838384e-3
};


#ifdef ANSIC
static float jntf( float nn, float xx )
#else
static float jntf( nn, xx )
double nn, xx;
#endif
{
float n, x, z, zz, z3;
float cbn, n23, cbtwo;
float ai, aip, bi, bip;	/* Airy functions */
float nk, fk, gk, pp, qq;
float F[5], G[4];
int k;

n = nn;
x = xx;
cbn = cbrt(n);
z = (x - n)/cbn;
cbtwo = cbrt( 2.0f );

/* Airy function */
zz = -cbtwo * z;
airy( zz, &ai, &aip, &bi, &bip );

/* polynomials in expansion */
zz = z * z;
z3 = zz * z;
F[0] = 1.0f;
F[1] = -z/5.0f;
F[2] = polevl( z3, PF2, 1 ) * zz;
F[3] = polevl( z3, PF3, 2 );
F[4] = polevl( z3, PF4, 3 ) * z;
G[0] = 0.3f * zz;
G[1] = polevl( z3, PG1, 1 );
G[2] = polevl( z3, PG2, 2 ) * z;
G[3] = polevl( z3, PG3, 2 ) * zz;
#if DEBUG
for( k=0; k<=4; k++ )
	printf( "F[%d] = %.5E\n", k, F[k] );
for( k=0; k<=3; k++ )
	printf( "G[%d] = %.5E\n", k, G[k] );
#endif
pp = 0.0f;
qq = 0.0f;
nk = 1.0f;
n23 = cbrt( n * n );

for( k=0; k<=4; k++ )
	{
	fk = F[k]*nk;
	pp += fk;
	if( k != 4 )
		{
		gk = G[k]*nk;
		qq += gk;
		}
#if DEBUG
	printf("fk[%d] %.5E, gk[%d] %.5E\n", k, fk, k, gk );
#endif
	nk /= n23;
	}

fk = cbtwo * ai * pp/cbn  +  cbrt(4.0f) * aip * qq/n;
return(fk);
}
