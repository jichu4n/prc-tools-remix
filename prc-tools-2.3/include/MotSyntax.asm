/* Since we're using the preprocessor anyway, let's avoid (most of) those
   percent signs, and teach it about the alternative names for a couple of
   the branch conditions, so we can write in something closer to Motorola
   syntax.  */

#define d0  %d0
#define d1  %d1
#define d2  %d2
#define d3  %d3
#define d4  %d4
#define d5  %d5
#define d6  %d6
#define d7  %d7
#define a0  %a0
#define a1  %a1
#define a2  %a2
#define a3  %a3
#define a4  %a4
#define a5  %a5
#define a6  %a6
#define a7  %a7
#define fp  %fp
#define sp  %sp
#define pc  %pc

	.macro jbhs label
	jbcc \label
	.endm

	.macro jblo label
	jbcs \label
	.endm

	.macro bhs label
	bcc \label
	.endm

	.macro blo label
	bcs \label
	.endm

	.macro dbhs reg,label
	dbcc \reg,\label
	.endm

	.macro dblo reg,label
	dbcs \reg,\label
	.endm
