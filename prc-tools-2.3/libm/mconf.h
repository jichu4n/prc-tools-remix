/*  mconf.h  */

/* Constant definitions for math error conditions
 */

#define DOMAIN		1	/* argument domain error */
#define SING		2	/* argument singularity */
#define OVERFLOW	3	/* overflow range error */
#define UNDERFLOW	4	/* underflow range error */
#define TLOSS		5	/* total loss of precision */
#define PLOSS		6	/* partial loss of precision */

#define EDOM		33
#define ERANGE		34

/* Type of computer */
/* define DEC 1 */
/* #define IBMPC 1 */
#define MIEEE 1
/* define UNK 1 */

#define ANSIC 1
/* get rid of this thing.  Takes too much space anyway */
#define mtherr(a,b)

#include "mathf.h"
