/*							atan.c
 *
 *	Inverse circular tangent
 *      (arctangent)
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, atan();
 *
 * y = atan( x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns radian angle between -pi/2 and +pi/2 whose tangent
 * is x.
 *
 * Range reduction is from four intervals into the interval
 * from zero to  tan( pi/8 ).  A polynomial approximates
 * the function in this basic interval.
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10, 10     100000      1.9e-7      4.1e-8
 *
 */
/*							atan2()
 *
 *	Quadrant correct inverse circular tangent
 *
 *
 *
 * SYNOPSIS:
 *
 * float x, y, z, atan2();
 *
 * z = atan2( y, x );
 *
 *
 *
 * DESCRIPTION:
 *
 * Returns radian angle whose tangent is y/x.
 * Define compile time symbol ANSIC = 1 for ANSI standard,
 * range -PI < z <= +PI, args (y,x); else ANSIC = 0 for range
 * 0 to 2PI, args (x,y).
 *
 *
 *
 * ACCURACY:
 *
 *                      Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      -10, 10     100000      1.9e-7      4.1e-8
 * See atan.c.
 *
 */

/*							atan.c */


/*
Cephes Math Library Release 2.2:  June, 1992
Copyright 1984, 1987, 1989, 1992 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/

/* Single precision circular arcsine
 * test interval: [-tan(pi/8), +tan(pi/8)]
 * trials: 10000
 * peak relative error: 7.7e-8
 * rms relative error: 2.9e-8
 */
#include "mconf.h"
extern float PIF, PIO2F, PIO4F;

#ifdef ANSIC
float atan( float xx )
#else
float atan(xx)
double xx;
#endif
{
float x, y, z;
int sign;

x = xx;

/* make argument positive and save the sign */
if( xx < 0.0f )
	{
	sign = -1;
	x = -xx;
	}
else
	{
	sign = 1;
	x = xx;
	}
/* range reduction */
if( x > 2.414213562373095f )  /* tan 3pi/8 */
	{
	y = PIO2F;
	x = -( 1.0f/x );
	}

else if( x > 0.4142135623730950f ) /* tan pi/8 */
	{
	y = PIO4F;
	x = (x-1.0f)/(x+1.0f);
	}
else
	y = 0.0f;

z = x * x;
y +=
((( 8.05374449538e-2f * z
  - 1.38776856032E-1f) * z
  + 1.99777106478E-1f) * z
  - 3.33329491539E-1f) * z * x
  + x;

if( sign < 0 )
	y = -y;

return( y );
}




#if ANSIC
float atan2( float y, float x )
#else
float atan2( x, y )
double x, y;
#endif
{
float z, w;
int code;


code = 0;

if( x < 0.0f )
	code = 2;
if( y < 0.0f )
	code |= 1;

if( x == 0.0f )
	{
	if( code & 1 )
		{
#if ANSIC
		return( -PIO2F );
#else
		return( 3.0f*PIO2F );
#endif
		}
	if( y == 0.0f )
		return( 0.0f );
	return( PIO2F );
	}

if( y == 0.0f )
	{
	if( code & 2 )
		return( PIF );
	return( 0.0f );
	}


switch( code )
	{
	default:
#if ANSIC
	case 0:
	case 1: w = 0.0f; break;
	case 2: w = PIF; break;
	case 3: w = -PIF; break;
#else
	case 0: w = 0.0f; break;
	case 1: w = 2.0f * PIF; break;
	case 2:
	case 3: w = PIF; break;
#endif
	}

z = atan( y/x );

return( w + z );
}

