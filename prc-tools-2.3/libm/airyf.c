/*							airy.c
 *
 *	Airy function
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, ai, aip, bi, bip;
 * int airy();
 *
 * airy( x, _&ai, _&aip, _&bi, _&bip );
 *
 *
 *
 * DESCRIPTION:
 *
 * Solution of the differential equation
 *
 *	y"(x) = xy.
 *
 * The function returns the two independent solutions Ai, Bi
 * and their first derivatives Ai'(x), Bi'(x).
 *
 * Evaluation is by power series summation for small x,
 * by rational minimax approximations for large x.
 *
 *
 *
 * ACCURACY:
 * Error criterion is absolute when function <= 1, relative
 * when function > 1, except * denotes relative error criterion.
 * For large negative x, the absolute error increases as x^1.5.
 * For large positive x, the relative error increases as x^1.5.
 *
 * Arithmetic  domain   function  # trials      peak         rms
 * IEEE        -10, 0     Ai        50000       7.0e-7      1.2e-7
 * IEEE          0, 10    Ai        50000       9.9e-6*     6.8e-7*
 * IEEE        -10, 0     Ai'       50000       2.4e-6      3.5e-7
 * IEEE          0, 10    Ai'       50000       8.7e-6*     6.2e-7*
 * IEEE        -10, 10    Bi       100000       2.2e-6      2.6e-7
 * IEEE        -10, 10    Bi'       50000       2.2e-6      3.5e-7
 *
 */
/*							airy.c */

/*
Cephes Math Library Release 2.2: June, 1992
Copyright 1984, 1987, 1989, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

#include "mconf.h"

static float c1 = 0.35502805388781723926f;
static float c2 = 0.258819403792806798405f;
static float sqrt3 = 1.732050807568877293527f;
static float sqpii = 5.64189583547756286948E-1f;
extern float PIF;

extern float MAXNUMF, MACHEPF;
#define MAXAIRY 25.77f

/* Note, these expansions are for double precision accuracy;
 * they have not yet been redesigned for single precision.
 */
static float AN[8] = {
  3.46538101525629032477e-1f,
  1.20075952739645805542e1f,
  7.62796053615234516538e1f,
  1.68089224934630576269e2f,
  1.59756391350164413639e2f,
  7.05360906840444183113e1f,
  1.40264691163389668864e1f,
  9.99999999999999995305e-1f,
};
static float AD[8] = {
  5.67594532638770212846e-1f,
  1.47562562584847203173e1f,
  8.45138970141474626562e1f,
  1.77318088145400459522e2f,
  1.64234692871529701831e2f,
  7.14778400825575695274e1f,
  1.40959135607834029598e1f,
  1.00000000000000000470e0f,
};


static float APN[8] = {
  6.13759184814035759225e-1f,
  1.47454670787755323881e1f,
  8.20584123476060982430e1f,
  1.71184781360976385540e2f,
  1.59317847137141783523e2f,
  6.99778599330103016170e1f,
  1.39470856980481566958e1f,
  1.00000000000000000550e0f,
};
static float APD[8] = {
  3.34203677749736953049e-1f,
  1.11810297306158156705e1f,
  7.11727352147859965283e1f,
  1.58778084372838313640e2f,
  1.53206427475809220834e2f,
  6.86752304592780337944e1f,
  1.38498634758259442477e1f,
  9.99999999999999994502e-1f,
};

static float BN16[5] = {
-2.53240795869364152689e-1f,
 5.75285167332467384228e-1f,
-3.29907036873225371650e-1f,
 6.44404068948199951727e-2f,
-3.82519546641336734394e-3f,
};
static float BD16[5] = {
/* 1.00000000000000000000e0,*/
-7.15685095054035237902e0f,
 1.06039580715664694291e1f,
-5.23246636471251500874e0f,
 9.57395864378383833152e-1f,
-5.50828147163549611107e-2f,
};

static float BPPN[5] = {
 4.65461162774651610328e-1f,
-1.08992173800493920734e0f,
 6.38800117371827987759e-1f,
-1.26844349553102907034e-1f,
 7.62487844342109852105e-3f,
};
static float BPPD[5] = {
/* 1.00000000000000000000e0,*/
-8.70622787633159124240e0f,
 1.38993162704553213172e1f,
-7.14116144616431159572e0f,
 1.34008595960680518666e0f,
-7.84273211323341930448e-2f,
};

static float AFN[9] = {
-1.31696323418331795333e-1f,
-6.26456544431912369773e-1f,
-6.93158036036933542233e-1f,
-2.79779981545119124951e-1f,
-4.91900132609500318020e-2f,
-4.06265923594885404393e-3f,
-1.59276496239262096340e-4f,
-2.77649108155232920844e-6f,
-1.67787698489114633780e-8f,
};
static float AFD[9] = {
/* 1.00000000000000000000e0,*/
 1.33560420706553243746e1f,
 3.26825032795224613948e1f,
 2.67367040941499554804e1f,
 9.18707402907259625840e0f,
 1.47529146771666414581e0f,
 1.15687173795188044134e-1f,
 4.40291641615211203805e-3f,
 7.54720348287414296618e-5f,
 4.51850092970580378464e-7f,
};

static float AGN[11] = {
  1.97339932091685679179e-2f,
  3.91103029615688277255e-1f,
  1.06579897599595591108e0f,
  9.39169229816650230044e-1f,
  3.51465656105547619242e-1f,
  6.33888919628925490927e-2f,
  5.85804113048388458567e-3f,
  2.82851600836737019778e-4f,
  6.98793669997260967291e-6f,
  8.11789239554389293311e-8f,
  3.41551784765923618484e-10f,
};
static float AGD[10] = {
/*  1.00000000000000000000e0,*/
  9.30892908077441974853e0f,
  1.98352928718312140417e1f,
  1.55646628932864612953e1f,
  5.47686069422975497931e0f,
  9.54293611618961883998e-1f,
  8.64580826352392193095e-2f,
  4.12656523824222607191e-3f,
  1.01259085116509135510e-4f,
  1.17166733214413521882e-6f,
  4.91834570062930015649e-9f,
};

static float APFN[9] = {
  1.85365624022535566142e-1f,
  8.86712188052584095637e-1f,
  9.87391981747398547272e-1f,
  4.01241082318003734092e-1f,
  7.10304926289631174579e-2f,
  5.90618657995661810071e-3f,
  2.33051409401776799569e-4f,
  4.08718778289035454598e-6f,
  2.48379932900442457853e-8f,
};
static float APFD[9] = {
/*  1.00000000000000000000e0,*/
  1.47345854687502542552e1f,
  3.75423933435489594466e1f,
  3.14657751203046424330e1f,
  1.09969125207298778536e1f,
  1.78885054766999417817e0f,
  1.41733275753662636873e-1f,
  5.44066067017226003627e-3f,
  9.39421290654511171663e-5f,
  5.65978713036027009243e-7f,
};

static float APGN[11] = {
-3.55615429033082288335e-2f,
-6.37311518129435504426e-1f,
-1.70856738884312371053e0f,
-1.50221872117316635393e0f,
-5.63606665822102676611e-1f,
-1.02101031120216891789e-1f,
-9.48396695961445269093e-3f,
-4.60325307486780994357e-4f,
-1.14300836484517375919e-5f,
-1.33415518685547420648e-7f,
-5.63803833958893494476e-10f,
};
static float APGD[11] = {
/*  1.00000000000000000000e0f,*/
  9.85865801696130355144e0f,
  2.16401867356585941885e1f,
  1.73130776389749389525e1f,
  6.17872175280828766327e0f,
  1.08848694396321495475e0f,
  9.95005543440888479402e-2f,
  4.78468199683886610842e-3f,
  1.18159633322838625562e-4f,
  1.37480673554219441465e-6f,
  5.79912514929147598821e-9f,
};

#define fabsf(x) ( (x) < 0.0f ? -(x) : (x) )

#ifdef ANSIC
float polevl(float, float *, int);
float p1evl(float, float *, int);
float sin(float), cos(float), exp(float), sqrt(float);

int airy( float xx, float *ai, float *aip, float *bi, float *bip )

#else
float polevl(), p1evl(), sin(), cos(), exp(), sqrt();

int airy( xx, ai, aip, bi, bip )
double xx;
float *ai, *aip, *bi, *bip;
#endif
{
float x, z, zz, t, f, g, uf, ug, k, zeta, theta;
int domflg;

x = xx;
domflg = 0;
if( x > MAXAIRY )
	{
	*ai = 0;
	*aip = 0;
	*bi = MAXNUMF;
	*bip = MAXNUMF;
	return(-1);
	}

if( x < -2.09f )
	{
	domflg = 15;
	t = sqrt(-x);
	zeta = -2.0f * x * t / 3.0f;
	t = sqrt(t);
	k = sqpii / t;
	z = 1.0f/zeta;
	zz = z * z;
	uf = 1.0f + zz * polevl( zz, AFN, 8 ) / p1evl( zz, AFD, 9 );
	ug = z * polevl( zz, AGN, 10 ) / p1evl( zz, AGD, 10 );
	theta = zeta + 0.25f * PIF;
	f = sin( theta );
	g = cos( theta );
	*ai = k * (f * uf - g * ug);
	*bi = k * (g * uf + f * ug);
	uf = 1.0f + zz * polevl( zz, APFN, 8 ) / p1evl( zz, APFD, 9 );
	ug = z * polevl( zz, APGN, 10 ) / p1evl( zz, APGD, 10 );
	k = sqpii * t;
	*aip = -k * (g * uf + f * ug);
	*bip = k * (f * uf - g * ug);
	return(0);
	}

if( x >= 2.09f )	/* cbrt(9) */
	{
	domflg = 5;
	t = sqrt(x);
	zeta = 2.0f * x * t / 3.0f;
	g = exp( zeta );
	t = sqrt(t);
	k = 2.0f * t * g;
	z = 1.0f/zeta;
	f = polevl( z, AN, 7 ) / polevl( z, AD, 7 );
	*ai = sqpii * f / k;
	k = -0.5f * sqpii * t / g;
	f = polevl( z, APN, 7 ) / polevl( z, APD, 7 );
	*aip = f * k;

	if( x > 8.3203353f )	/* zeta > 16 */
		{
		f = z * polevl( z, BN16, 4 ) / p1evl( z, BD16, 5 );
		k = sqpii * g;
		*bi = k * (1.0f + f) / t;
		f = z * polevl( z, BPPN, 4 ) / p1evl( z, BPPD, 5 );
		*bip = k * t * (1.0f + f);
		return(0);
		}
	}

f = 1.0f;
g = x;
t = 1.0f;
uf = 1.0f;
ug = x;
k = 1.0f;
z = x * x * x;
while( t > MACHEPF )
	{
	uf *= z;
	k += 1.0f;
	uf /=k;
	ug *= z;
	k += 1.0f;
	ug /=k;
	uf /=k;
	f += uf;
	k += 1.0f;
	ug /=k;
	g += ug;
	t = fabsf(uf/f);
	}
uf = c1 * f;
ug = c2 * g;
if( (domflg & 1) == 0 )
	*ai = uf - ug;
if( (domflg & 2) == 0 )
	*bi = sqrt3 * (uf + ug);

/* the deriviative of ai */
k = 4.0f;
uf = x * x/2.0f;
ug = z/3.0f;
f = uf;
g = 1.0f + ug;
uf /= 3.0f;
t = 1.0f;

while( t > MACHEPF )
	{
	uf *= z;
	ug /=k;
	k += 1.0f;
	ug *= z;
	uf /=k;
	f += uf;
	k += 1.0f;
	ug /=k;
	uf /=k;
	g += ug;
	k += 1.0f;
	t = fabsf(ug/g);
	}

uf = c1 * f;
ug = c2 * g;
if( (domflg & 4) == 0 )
	*aip = uf - ug;
if( (domflg & 8) == 0 )
	*bip = sqrt3 * (uf + ug);
return(0);
}
