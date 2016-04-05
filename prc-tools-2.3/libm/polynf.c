/*							polyn.c
 *							polyrf.c
 * Arithmetic operations on polynomials
 *
 * In the following descriptions a, b, c are polynomials of degree
 * na, nb, nc respectively.  The degree of a polynomial cannot
 * exceed a run-time value MAXPOLF.  An operation that attempts
 * to use or generate a polynomial of higher degree may produce a
 * result that suffers truncation at degree MAXPOL.  The value of
 * MAXPOL is set by calling the function
 *
 *     polini( maxpol );
 *
 * where maxpol is the desired maximum degree.  This must be
 * done prior to calling any of the other functions in this module.
 * Memory for internal temporary polynomial storage is allocated
 * by polini().
 *
 * Each polynomial is represented by an array containing its
 * coefficients, together with a separately declared integer equal
 * to the degree of the polynomial.  The coefficients appear in
 * ascending order; that is,
 *
 *                                        2                      na
 * a(x)  =  a[0]  +  a[1] * x  +  a[2] * x   +  ...  +  a[na] * x  .
 *
 *
 *
 * sum = poleva( a, na, x );	Evaluate polynomial a(t) at t = x.
 * polprt( a, na, D );		Print the coefficients of a to D digits.
 * polclr( a, na );		Set a identically equal to zero, up to a[na].
 * polmov( a, na, b );		Set b = a.
 * poladd( a, na, b, nb, c );	c = b + a, nc = max(na,nb)
 * polsub( a, na, b, nb, c );	c = b - a, nc = max(na,nb)
 * polmul( a, na, b, nb, c );	c = b * a, nc = na+nb
 *
 *
 * Division:
 *
 * i = poldiv( a, na, b, nb, c );	c = b / a, nc = MAXPOL
 *
 * returns i = the degree of the first nonzero coefficient of a.
 * The computed quotient c must be divided by x^i.  An error message
 * is printed if a is identically zero.
 *
 *
 * Change of variables:
 * If a and b are polynomials, and t = a(x), then
 *     c(t) = b(a(x))
 * is a polynomial found by substituting a(x) for t.  The
 * subroutine call for this is
 *
 * polsbtf( a, na, b, nb, c );
 *
 *
 * Notes:
 * poldiv() is an integer routine; poleva() is float.
 * Any of the arguments a, b, c may refer to the same array.
 *
 */

#ifndef NULL
#define NULL 0
#endif
#include "mconf.h"

#ifdef ANSIC
void printf(), sprintf(), exit();
void free(void *);
void *malloc(int);
#else
void printf(), sprintf(), free(), exit();
void *malloc();
#endif
/* near pointer version of malloc() */
/*#define malloc _nmalloc*/
/*#define free _nfree*/

/* Pointers to internal arrays.  Note poldiv() allocates
 * and deallocates some temporary arrays every time it is called.
 */
static float *pt1 = 0;
static float *pt2 = 0;
static float *pt3 = 0;

/* Maximum degree of polynomial. */
int MAXPOLF = 0;
extern int MAXPOLF;

/* Number of bytes (chars) in maximum size polynomial. */
static int psize = 0;


/* Initialize max degree of polynomials
 * and allocate temporary storage.
 */
#ifdef ANSIC
void polini( int maxdeg )
#else
int polini( maxdeg )
int maxdeg;
#endif
{

MAXPOLF = maxdeg;
psize = (maxdeg + 1) * sizeof(float);

/* Release previously allocated memory, if any. */
if( pt3 )
	free(pt3);
if( pt2 )
	free(pt2);
if( pt1 )
	free(pt1);

/* Allocate new arrays */
pt1 = (float * )malloc(psize); /* used by polsbt */
pt2 = (float * )malloc(psize); /* used by polsbt */
pt3 = (float * )malloc(psize); /* used by polmul */

/* Report if failure */
if( (pt1 == NULL) || (pt2 == NULL) || (pt3 == NULL) )
	{
	mtherr( "polini", ERANGE );
	exit(1);
	}
#if !ANSIC
return 0;
#endif
}



/* Print the coefficients of a, with d decimal precision.
 */
static char *form = "abcdefghijk";

#ifdef ANSIC
void polprt( float *a, int na, int d )
#else
int polprt( a, na, d )
float a[];
int na, d;
#endif
{
int i, j, d1;
char *p;

/* Create format descriptor string for the printout.
 * Do this partly by hand, since sprintf() may be too
 * bug-ridden to accomplish this feat by itself.
 */
p = form;
*p++ = '%';
d1 = d + 8;
(void )sprintf( p, "%d ", d1 );
p += 1;
if( d1 >= 10 )
	p += 1;
*p++ = '.';
(void )sprintf( p, "%d ", d );
p += 1;
if( d >= 10 )
	p += 1;
*p++ = 'e';
*p++ = ' ';
*p++ = '\0';


/* Now do the printing.
 */
d1 += 1;
j = 0;
for( i=0; i<=na; i++ )
	{
/* Detect end of available line */
	j += d1;
	if( j >= 78 )
		{
		printf( "\n" );
		j = d1;
		}
	printf( form, a[i] );
	}
printf( "\n" );
#if !ANSIC
return 0;
#endif
}



/* Set a = 0.
 */
#ifdef ANSIC
void polclr( register float *a, int n )
#else
int polclr( a, n )
register float *a;
int n;
#endif
{
int i;

if( n > MAXPOLF )
	n = MAXPOLF;
for( i=0; i<=n; i++ )
	*a++ = 0.0f;
#if !ANSIC
return 0;
#endif
}



/* Set b = a.
 */
#ifdef ANSIC
void polmov( register float *a, int na, register float *b )
#else
int polmov( a, na, b )
register float *a, *b;
int na;
#endif
{
int i;

if( na > MAXPOLF )
	na = MAXPOLF;

for( i=0; i<= na; i++ )
	{
	*b++ = *a++;
	}
#if !ANSIC
return 0;
#endif
}


/* c = b * a.
 */
#ifdef ANSIC
void polmul( float a[], int na, float b[], int nb, float c[] )
#else
int polmul( a, na, b, nb, c )
float a[], b[], c[];
int na, nb;
#endif
{
int i, j, k, nc;
float x;

nc = na + nb;
polclr( pt3, MAXPOLF );

for( i=0; i<=na; i++ )
	{
	x = a[i];
	for( j=0; j<=nb; j++ )
		{
		k = i + j;
		if( k > MAXPOLF )
			break;
		pt3[k] += x * b[j];
		}
	}

if( nc > MAXPOLF )
	nc = MAXPOLF;
for( i=0; i<=nc; i++ )
	c[i] = pt3[i];
#if !ANSIC
return 0;
#endif
}



 
/* c = b + a.
 */
#ifdef ANSIC
void poladd( float a[], int na, float b[], int nb, float c[] )
#else
int poladd( a, na, b, nb, c )
float a[], b[], c[];
int na, nb;
#endif
{
int i, n;


if( na > nb )
	n = na;
else
	n = nb;

if( n > MAXPOLF )
	n = MAXPOLF;

for( i=0; i<=n; i++ )
	{
	if( i > na )
		c[i] = b[i];
	else if( i > nb )
		c[i] = a[i];
	else
		c[i] = b[i] + a[i];
	}
#if !ANSIC
return 0;
#endif
}

/* c = b - a.
 */
#ifdef ANSIC
void polsub( float a[], int na, float b[], int nb, float c[] )
#else
int polsub( a, na, b, nb, c )
float a[], b[], c[];
int na, nb;
#endif
{
int i, n;


if( na > nb )
	n = na;
else
	n = nb;

if( n > MAXPOLF )
	n = MAXPOLF;

for( i=0; i<=n; i++ )
	{
	if( i > na )
		c[i] = b[i];
	else if( i > nb )
		c[i] = -a[i];
	else
		c[i] = b[i] - a[i];
	}
#if !ANSIC
return 0;
#endif
}



/* c = b/a
 */
#ifdef ANSIC
int poldiv( float a[], int na, float b[], int nb, float c[] )
#else
int poldiv( a, na, b, nb, c )
float a[], b[], c[];
int na, nb;
#endif
{
float quot;
float *ta, *tb, *tq;
int i, j, k, sing;

sing = 0;

/* Allocate temporary arrays.  This would be quicker
 * if done automatically on the stack, but stack space
 * may be hard to obtain on a small computer.
 */
ta = (float * )malloc( psize );
polclr( ta, MAXPOLF );
polmov( a, na, ta );

tb = (float * )malloc( psize );
polclr( tb, MAXPOLF );
polmov( b, nb, tb );

tq = (float * )malloc( psize );
polclr( tq, MAXPOLF );

/* What to do if leading (constant) coefficient
 * of denominator is zero.
 */
if( a[0] == 0.0f )
	{
	for( i=0; i<=na; i++ )
		{
		if( ta[i] != 0.0f )
			goto nzero;
		}
	mtherr( "poldiv", SING );
	goto done;

nzero:
/* Reduce the degree of the denominator. */
	for( i=0; i<na; i++ )
		ta[i] = ta[i+1];
	ta[na] = 0.0f;

	if( b[0] != 0.0f )
		{
/* Optional message:
		printf( "poldiv singularity, divide quotient by x\n" );
*/
		sing += 1;
		}
	else
		{
/* Reduce degree of numerator. */
		for( i=0; i<nb; i++ )
			tb[i] = tb[i+1];
		tb[nb] = 0.0f;
		}
/* Call self, using reduced polynomials. */
	sing += poldiv( ta, na, tb, nb, c );
	goto done;
	}

/* Long division algorithm.  ta[0] is nonzero.
 */
for( i=0; i<=MAXPOLF; i++ )
	{
	quot = tb[i]/ta[0];
	for( j=0; j<=MAXPOLF; j++ )
		{
		k = j + i;
		if( k > MAXPOLF )
			break;
		tb[k] -= quot * ta[j];
		}
	tq[i] = quot;
	}
/* Send quotient to output array. */
polmov( tq, MAXPOLF, c );

done:

/* Restore allocated memory. */
free(tq);
free(tb);
free(ta);
return( sing );
}




/* Change of variables
 * Substitute a(y) for the variable x in b(x).
 * x = a(y)
 * c(x) = b(x) = b(a(y)).
 */

#ifdef ANSIC
void polsbt( float a[], int na, float b[], int nb, float c[] )
#else
int polsbt( a, na, b, nb, c )
float a[], b[], c[];
int na, nb;
#endif
{
int i, j, k, n2;
float x;

/* 0th degree term:
 */
polclr( pt1, MAXPOLF );
pt1[0] = b[0];

polclr( pt2, MAXPOLF );
pt2[0] = 1.0f;
n2 = 0;

for( i=1; i<=nb; i++ )
	{
/* Form ith power of a. */
	polmul( a, na, pt2, n2, pt2 );
	n2 += na;
	x = b[i];
/* Add the ith coefficient of b times the ith power of a. */
	for( j=0; j<=n2; j++ )
		{
		if( j > MAXPOLF )
			break;
		pt1[j] += x * pt2[j];
		}
	}

k = n2 + nb;
if( k > MAXPOLF )
	k = MAXPOLF;
for( i=0; i<=k; i++ )
	c[i] = pt1[i];
#if !ANSIC
return 0;
#endif
}




/* Evaluate polynomial a(t) at t = x.
 */
#ifdef ANSIC
float poleva( float *a, int na, float xx )
#else
float poleva( a, na, xx )
float a[];
int na;
double xx;
#endif
{
float x, s;
int i;

x = xx;
s = a[na];
for( i=na-1; i>=0; i-- )
	{
	s = s * x + a[i];
	}
return(s);
}

