/* ctype.c: ISO/IEC 9899:1999  7.4  Character handling.

   Placed in the public domain by John Marshall.  */

#if __GNUC__ < 3 && defined Lctype
/* FIXME When m68k-palmos-gcc is based on GCC 3.x and the "extern const data
   still goes in .data" bug is fixed, this workaround can be removed.  */
#define _NEED_HONEST_CTYPE_DECLS
#endif

#include "ctype.h"

#ifdef Lctype

/* For each character, _Ctype[] contains an ORed-together set of the
   following flags and, in the 0x01 and 0x02 bits (which we'll call O),
   an index into a table of differences used for case conversion.  */

#define L 0x04	/* Lowercase letter	a-z, accented lowercase letters */
#define U 0x08	/* Uppercase letter	A-Z, accented uppercase letters */
#define D 0x10	/* Hexadecimal digit	0-9 A-F a-f */
#define P 0x20	/* Printable character	anything with a non-stupid glyph */
#define S 0x40	/* Horizontal w.space	SP \t NBSP NUMSP */
#define C 0x80	/* Control-char w.space	\t \n \v \f \r */

/* This compact encoding depends on several properties of the Palm OS
   character set:

   * It happens to have some uncased letters (the feminine and masculine
     ordinal indicators) but no both-cased letters.  So we will consider
     having both L and U set to mean an uncased letter (oddly enough).

   * It has very few interesting "control characters".  In fact the only
     characters that are really control characters are the (non-printing)
     whitespace characters.  This allows us to merge the two concepts
     (control char; most whitespace) into one flag, C.

   * We've cheated with isgraph().  The Standard says that isgraph() is
     the same as isprint() except for ' '.  In particular, isgraph(NBSP)
     is supposed to be true, even though NBSP, like SP, has no visible
     glyph.  Somewhat more logically, the convention on Palm OS seems
     to be for isgraph() to measure whether a printable character has
     a visible glyph, and we've implemented it so that it differs from
     isprint() for all space-like characters: SP, NBSP, and NUMSP.

   * Pairs of corresponding upper- and lowercase letters differ by only
     three different amounts:  most are like the familiar 'a' - 'A' = 32,
     and only two other differences appear.  These four possibilities
     (three real offsets, and an offset of 0 for all the non-letters
     that are left unchanged by tolower/toupper) can be encoded in the
     remaining two bits, O.

   Given the flag definitions above and these pieces of luck, we can
   implement all the classification functions:

	isalnum = L|U|D			islower = L but not U
	isalpha = L|U			isupper = U but not L
	isdigit = D but not L|U		isxdigit= D

	isblank = S			isprint = P
	isspace = S|C			iscntrl = C

	isgraph = P but not S
	ispunct = P but not L|U|D|S = P but not L|U|D|S|C|O

   The index within OFFSETS of the appropriate difference for each character
   affected by tolower() or toupper() is stored in O.  Most characters are
   unaffected by these functions, so have O = 0 (since OFFSETS[0] = 0).

   The only characters affected by tolower() are uppercase letters; in
   particular, they do not have their L flag set.  Thus if we mask off all
   bits but O|L, we will get an index in the range {0--7}.  For lowercase
   (and uncased) letters which should not be affected, the index will be
   {4--7}; for others, the index will be the correct index into OFFSETS.

   Similarly, for toupper(), by masking off all bits but O|U we get an index
   in the range {0--3, 8--11}.  For uppercase (and uncased) letters, the index
   will be {8--11}; for others, it will be the correct index info OFFSETS.

   Thus an array with _Cconv[0--3] = OFFSETS and _Cconv[4--11] = 0 provides
   the differences (usually 0!) needed to add to or subtract from each
   character so that we can implement the case conversion functions:

	tolower difference = _Cconv[O|L]
	toupper difference = _Cconv[O|U]

   See the (somewhat obfuscated) definitions in <ctype.h> to see how all this
   translates into code.  */

#define OFFSETS  0, 16, 32, 96

#define ASCII \
	 /*	0	1	2	3	4	5	6	7 */ \
/* 00-07 */	0,	0,	0,	0,	0,	0,	0,	0,   \
/* 08-0f */	0,    S|C,	C,	C,	C,	C,	0,	0,   \
/* 10-17 */	0,	0,	0,	0,	P,	P,	P,	P,   \
/* 18-1f */	P,    S|P,	P,	0,	0,	0,	0,	0,   \
/* SP- ' */   S|P,	P,	P,	P,	P,	P,	P,	P,   \
/* ( - / */	P,	P,	P,	P,	P,	P,	P,	P,   \
/* 0 - 7 */   D|P,    D|P,    D|P,    D|P,    D|P,    D|P,    D|P,    D|P,   \
/* 8 - ? */   D|P,    D|P,	P,	P,	P,	P,	P,	P,   \
/* @ - G */	P,D|2|U|P,D|2|U|P,D|2|U|P,D|2|U|P,D|2|U|P,D|2|U|P,  2|U|P,   \
/* H - O */ 2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,   \
/* P - W */ 2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,   \
/* X - _ */ 2|U|P,  2|U|P,  2|U|P,	P,	P,	P,	P,	P,   \
/* ` - g */	P,D|2|L|P,D|2|L|P,D|2|L|P,D|2|L|P,D|2|L|P,D|2|L|P,  2|L|P,   \
/* h - o */ 2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,   \
/* p - w */ 2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,   \
/* x -7f */ 2|L|P,  2|L|P,  2|L|P,	P,	P,	P,	P,	0

#define LATIN1(ff) \
	 /*	0	1	2	3	4	5	6	7 */ \
/* 80-87 */	P,	0,	P,	P,	P,	P,	P,	P,   \
/* 88-8f */	P,	P,  1|U|P,	P,  1|U|P,	P,	P,	P,   \
/* 90-97 */	P,	P,	P,	P,	P,	P,	P,	P,   \
/* 98-9f */	P,	P,  1|L|P,	P,  1|L|P,	0,	0,  3|U|P,   \
/* a0-a7 */   S|P,	P,	P,	P,	P,	P,	P,	P,   \
/* a8-af */	P,	P,  L|U|P,	P,	P,	P,	P,	P,   \
/* b0-b7 */	P,	P,	P,	P,	P,	P,	P,	P,   \
/* b8-bf */	P,	P,  L|U|P,	P,	P,	P,	P,	P,   \
/* À - Ç */ 2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,   \
/* È - Ï */ 2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,   \
/* Ð - × */ 2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,	P,   \
/* Ø - ß */ 2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,  2|U|P,    L|P,   \
/* à - ç */ 2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,   \
/* è - ï */ 2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,   \
/* ð - ÷ */ 2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,	P,   \
/* ø - ÿ */ 2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,  2|L|P,     ff

#define FF  3|L|P  /* The entry for 0xff, "ÿ".  */

/* _Cconv[] must have 12 entries:  four from OFFSETS followed by eight zeros.
   _Ctype[] must have an entry for each index that might be a valid character,
   and also for EOF (-1).  The following is non-portable because it assumes
   that _Cconv[] and _Ctype[] will be contiguous in memory.  */

#ifdef __CHAR_UNSIGNED__
/* Here we take advantage of the fact that ASCII begins with ample zeros;
   the explicit "0" is there to provide the right value for _Ctype[EOF].  */
const unsigned char _Cconv[] = { OFFSETS, 0 };
const unsigned char _Ctype[] = { ASCII, LATIN1 (FF) };
#else
/* Here it is useful to ensure that Latin-1 characters get the right answers
   whether they are presented as signed chars (with values less than 0) or
   correctly converted to unsigned.  But you must present '\xff' correctly,
   because -1 is superseded by EOF.  */
const unsigned char _Cconv[] = { OFFSETS, 0,0,0,0, 0,0,0,0, LATIN1 (0) };
const unsigned char _Ctype[] = { ASCII, LATIN1 (FF) };
#endif

#endif
#ifdef Lisalnum
int (isalnum) (int c) { return isalnum (c); }
#endif
#ifdef Lisalpha
int (isalpha) (int c) { return isalpha (c); }
#endif
#ifdef Lisblank
int (isblank) (int c) { return isblank (c); }
#endif
#ifdef Liscntrl
int (iscntrl) (int c) { return iscntrl (c); }
#endif
#ifdef Lisdigit
int (isdigit) (int c) { return isdigit (c); }
#endif
#ifdef Lisgraph
int (isgraph) (int c) { return isgraph (c); }
#endif
#ifdef Lislower
int (islower) (int c) { return islower (c); }
#endif
#ifdef Lisprint
int (isprint) (int c) { return isprint (c); }
#endif
#ifdef Lispunct
int (ispunct) (int c) { return ispunct (c); }
#endif
#ifdef Lisspace
int (isspace) (int c) { return isspace (c); }
#endif
#ifdef Lisupper
int (isupper) (int c) { return isupper (c); }
#endif
#ifdef Lisxdigit
int (isxdigit) (int c) { return isxdigit (c); }
#endif
#ifdef Ltolower
int (tolower) (int c) { return tolower (c); }
#endif
#ifdef Ltoupper
int (toupper) (int c) { return toupper (c); }
#endif
