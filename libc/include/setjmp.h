/* setjmp.h:  Prototypes and defines for m68k setjmp/longjmp
 *
 * (c) Dionne & Associates 1995
 * (c) 1995, DKG Display Tech
 */

#ifndef _332_SETJMP_H_
#define _332_SETJMP_H_

typedef struct jmpbuf {
	unsigned int __j0;	/* 0  */
	unsigned int __j1;
	unsigned int sp;	/* 8  */
	unsigned int pc;	/* 12 */
	unsigned int __j3;
	unsigned int d2;	/* 20 */
        unsigned int d3;
        unsigned int d4;
        unsigned int d5;
        unsigned int d6;
        unsigned int d7;
        unsigned int a2;
        unsigned int a3;
        unsigned int a4;
        unsigned int a5;
        unsigned int fp;
} jmpbuf;

int setjmp(jmpbuf *jp);
int longjmp(jmpbuf *jp, int ret);

#endif /* _332_SETJMP_H_ */
