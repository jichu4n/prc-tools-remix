/* Definitions of target machine for GNU compiler.  "naked" 68020,
   COFF object files and debugging, version.
   Copyright (C) 1994 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include "m68k/m68k-coff.h"
#define PALMOS  1

/* A5 (global world) and A7 (stack pointer) are always fixed.
   A6 (frame pointer) will be saved as appropriate by gcc.
   A4 (extra PIC register) will be fixed if necessary by -mown-gp.  */
#undef FIXED_REGISTERS
#define FIXED_REGISTERS		\
 {/* Data registers.  */	\
  0, 0, 0, 0, 0, 0, 0, 0,	\
				\
  /* Address registers.  */	\
  0, 0, 0, 0, 0, 1, 0, 1,	\
				\
  /* Floating point registers	\
     (if available).  */	\
  0, 0, 0, 0, 0, 0, 0, 0 }

#undef CALL_USED_REGISTERS
#define CALL_USED_REGISTERS	\
 {1, 1, 1, 0, 0, 0, 0, 0,	\
  1, 1, 0, 0, 0, 1, 0, 1,	\
  1, 1, 0, 0, 0, 0, 0, 0 }

/* Normally the PIC register is A5; if we're building a GLib or some such,
   it'll be A4.  (The documentation doesn't explicitly allow this to be a
   variable -- for example, CONDITIONAL_REGISTER_USAGE is too late to set
   it.  We live in hope.)  */
extern int palmos_pic_reg;
#undef PIC_OFFSET_TABLE_REGNUM
#define PIC_OFFSET_TABLE_REGNUM  (palmos_pic_reg)

#undef CONDITIONAL_REGISTER_USAGE
#define CONDITIONAL_REGISTER_USAGE					\
  {									\
    if (TARGET_OWN_GP)							\
      {									\
	fixed_regs[PIC_OFFSET_TABLE_REGNUM] = 1;			\
	call_used_regs[PIC_OFFSET_TABLE_REGNUM] = 1;			\
	/* See also SUBTARGET_OVERRIDE_OPTIONS.  */			\
      }									\
  }

/* Make the prologue and epilogue save/restore A4 if necessary even though
   it's a call-used register.  */
#undef EXTRA_REGISTER_SAVE
#define EXTRA_REGISTER_SAVE(REGNO)					\
  (TARGET_OWN_GP && (REGNO) == PIC_OFFSET_TABLE_REGNUM			\
   && current_function_decl						\
   && lookup_attribute ("owngp",					\
		        DECL_MACHINE_ATTRIBUTES (current_function_decl)))

/* Under PalmOS/m68k scalars are returned in D0, and pointers
   are returned in A0.  We do so because CodeWarrior does.  */
#undef FUNCTION_VALUE
#define FUNCTION_VALUE(VALTYPE, FUNC)  \
  gen_rtx_REG (TYPE_MODE (VALTYPE),    \
	       (POINTER_TYPE_P(VALTYPE) && TARGET_RET_PTRS_A0) ? 8 : 0)

/* 1 if N is a possible register number for a function value.
   On the 68000, D0/A0 are the only registers thus used.  */
#undef FUNCTION_VALUE_REGNO_P
#define FUNCTION_VALUE_REGNO_P(N) ((N) == 0 || ((N) == 8 && TARGET_RET_PTRS_A0))

/* Functions which return large structures get the address
   to place the wanted value from a hidden parameter.  */

#undef  PCC_STATIC_STRUCT_RETURN
#undef  STRUCT_VALUE_REGNUM
#define STRUCT_VALUE 0
#define STRUCT_VALUE_INCOMING 0


#define MASK_DEBUG_LABELS	8192
#define TARGET_DEBUG_LABELS	(target_flags & MASK_DEBUG_LABELS)

#define MASK_PCREL		16384
#define TARGET_PCREL		(target_flags & MASK_PCREL)

#define MASK_OWN_GP		32768
#define TARGET_OWN_GP		(target_flags & MASK_OWN_GP)

#define MASK_EXTRALOGUES	65536
#define TARGET_EXTRALOGUES	(target_flags & MASK_EXTRALOGUES)

#define MASK_RET_PTRS_A0	131072
#define TARGET_RET_PTRS_A0	(target_flags & MASK_RET_PTRS_A0)

#undef SUBTARGET_SWITCHES
#define SUBTARGET_SWITCHES			\
   { "debug-labels", MASK_DEBUG_LABELS },	\
   { "no-debug-labels", -MASK_DEBUG_LABELS },	\
   { "pcrel", MASK_PCREL },			\
   { "no-pcrel", -MASK_PCREL },			\
   { "own-gp", MASK_OWN_GP },			\
   { "no-own-gp", -MASK_OWN_GP },		\
   { "extralogues", MASK_EXTRALOGUES },		\
   { "no-extralogues", -MASK_EXTRALOGUES },	\
   { "experimental-return-reg-d0", -MASK_RET_PTRS_A0 }, \
   { "no-experimental-return-reg-d0", MASK_RET_PTRS_A0 },

/* Target defaults are -mpcrel -mshort -m68000 -msoft-float.  */
#undef TARGET_DEFAULT
#define TARGET_DEFAULT	(MASK_SHORT | MASK_PCREL | MASK_RET_PTRS_A0)

#undef SUBTARGET_OVERRIDE_OPTIONS
#define SUBTARGET_OVERRIDE_OPTIONS					\
  {									\
    if (TARGET_PCREL)							\
      flag_pic = 1;							\
    if (TARGET_OWN_GP)							\
      target_flags |= MASK_EXTRALOGUES;					\
    palmos_pic_reg = (TARGET_OWN_GP)? 12 : 13;			\
  }

/* Always disallow function-cse for calls to callseq functions.  */
#define FORBID_FUNCTION_CSE_P(EXP)					\
  ((GET_CODE (EXP) == SYMBOL_REF && (XSTR ((EXP), 0))[0] == '=')	\
   || GET_CODE (EXP) == UNSPEC)


/* Make jumping to the start of a code resource work even if there are
   string constants and the like in the first function.  */
/* DAMMIT it doesn't work: there's a bug with egcs still emitting constants
   for inline functions that are declared but not called.
define CONSTANT_POOL_BEFORE_FUNCTION 0
define CONSTANT_AFTER_FUNCTION_P(EXP) 1
*/

/* Some Palm OS headers in TOOL_INCLUDE_DIR need to #include_next their
   corresponding header in CROSS_INCLUDE_DIR, so we need to reorder them
   from the default.  */
#define INCLUDE_DEFAULTS			\
  {						\
    { GCC_INCLUDE_DIR, "GCC", 0, 0 },		\
    { TOOL_INCLUDE_DIR, "BINUTILS", 0, 1 },	\
    { CROSS_INCLUDE_DIR, 0, 0, 0 },		\
    { 0, 0, 0, 0 }				\
  }

/* Palm OS headers are C++-aware.  */
#define NO_IMPLICIT_EXTERN_C

#define TARGET_MEM_FUNCTIONS

#undef PROMOTE_PROTOTYPES

#undef FUNCTION_ARG_PADDING
#define FUNCTION_ARG_PADDING(MODE, TYPE) upward

/* Make system function calls work even with -mnoshort ints.  */
#undef PARM_BOUNDARY
#define PARM_BOUNDARY 16

/* It is useful to be able to specify more than the usual m68k amount of
   alignment if you want to interact with ARM subroutines.  */
#define MAX_OFILE_ALIGNMENT  32

#undef ASM_OUTPUT_ALIGN
#define ASM_OUTPUT_ALIGN(FILE, LOG)				\
  do								\
    {								\
      int amount = 1 << (LOG);					\
      if (amount > 2)						\
	fprintf (FILE, "\t.balign %d\n", amount);		\
      else if (amount == 2)					\
	fprintf (FILE, "\t.even\n");				\
    } while (0)

#define DEFAULT_SHORT_ENUMS 1

#undef CPP_PREDEFINES
#define CPP_PREDEFINES "\
-Dm68000 -Dmc68000 -Dm68k -D__palmos__ \
-Asystem(palmos) -Acpu(m68k) -Amachine(pilot)"

/* -l makes the assembler generate .w branches only */
#undef ASM_SPEC
#define ASM_SPEC "-mno-68881 -m68000 -l"

#undef CC1_SPEC
#define CC1_SPEC "-fpic -msoft-float -m68000 %{!Wmultichar:-Wno-multichar}"

#undef CC1PLUS_SPEC
#define CC1PLUS_SPEC ""

#undef LINK_SPEC
#define LINK_SPEC \
  "--embedded-relocs --no-check-sections -N %{!static:-dy}"

#undef STARTFILE_SPEC
#define STARTFILE_SPEC \
  "%{!shared:crt0.o%s} %{shared:scrt0.o%s} %{g:gdbstub.o%s}"

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "-lcrt"

#undef LIB_SPEC
#define LIB_SPEC "%{!shared:%{g*:-lg} %{!p:%{!pg:-lc}}%{p:-lc_p}%{pg:-lc_p}}"

#undef CPP_SUBTARGET_SPEC
#define CPP_SUBTARGET_SPEC \
  "%{mown-gp:-D__OWNGP__} %{mextralogues:-D__EXTRALOGUES__} \
   %{!mnoshort:-D__INT_MAX__=32767}"

#undef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS \
  { "trad_capable_cpp", \
      "cpp0 %{traditional} %{ftraditional:-traditional}\
       %{traditional-cpp:-traditional}" }, \
  { "cpp_options", \
      "%{ansi:-std=c89} %{std*} %{nostdinc*} %{C} %{v} %{A*} %{I*} %{P} %{$} %I\
       %{!no-gcc:-D__GNUC__=%v1 -D__GNUC_MINOR__=%v2}\
       %{ansi|std=*:%{!std=gnu*:-trigraphs -D__STRICT_ANSI__}}\
       %{!undef:%{!ansi:%{!std=*:%p}%{std=gnu*:%p}} %P} %{trigraphs} %{remap}\
       %c %{fleading-underscore} %{fno-leading-underscore} %{g*} %{W*} %{w}\
       %{pedantic*} %{H} %C %{D*} %{U*} %{i*} %Z %i" }, \
  { "cpp_debug_options", "%{d*}" },

extern int palmos_valid_machine_decl_attribute ();
#define VALID_MACHINE_DECL_ATTRIBUTE(DECL, ATTRIBUTES, ID, ARGS)	\
  palmos_valid_machine_decl_attribute (DECL, ATTRIBUTES, ID, ARGS)

#if 0
extern union tree_node *palmos_wibble();
#define MERGE_MACHINE_DECL_ATTRIBUTES(OLDDECL, NEWDECL)			\
  palmos_wibble (0, merge_attributes (palmos_wibble (2, DECL_MACHINE_ATTRIBUTES (OLDDECL)), palmos_wibble (1, DECL_MACHINE_ATTRIBUTES (NEWDECL))))
#endif

#define READONLY_DATA_SECTION  curfunc_section
#define JUMP_TABLES_IN_TEXT_SECTION  1

/* Unfortunately, coff.h offers no subtarget macros for this.  */
#undef EXTRA_SECTION_FUNCTIONS
#define EXTRA_SECTION_FUNCTIONS						\
  CTORS_SECTION_FUNCTION						\
  DTORS_SECTION_FUNCTION						\
  CURFUNC_SECTION_FUNCTION

#define CURFUNC_SECTION_FUNCTION					\
void									\
curfunc_section ()							\
{									\
  function_section (current_function_decl);				\
}

#define RECALL_CONSTANT_NAME_SECTION_INFO

extern void palmos_encode_section_info ();
#define ENCODE_SECTION_INFO(decl)					\
do									\
  {									\
    rtx sym;								\
    int decl_code = TREE_CODE (decl);					\
    if ((decl_code == VAR_DECL || decl_code == FUNCTION_DECL)		\
	&& (sym = DECL_RTL (decl))					\
	&& GET_CODE (sym) == MEM					\
	&& GET_CODE (XEXP (sym, 0)) == SYMBOL_REF)			\
      {									\
	if (decl_code == VAR_DECL && TREE_ASM_WRITTEN (decl))		\
	  SYMBOL_REF_FLAG (XEXP (sym, 0)) = DECL_IN_TEXT_SECTION (decl);\
	else if (decl_code == FUNCTION_DECL)				\
	  {								\
	    SYMBOL_REF_FLAG (XEXP (sym, 0)) = 1;			\
	    palmos_encode_section_info (sym, decl, 0);			\
	  }								\
	else								\
	  SYMBOL_REF_FLAG (XEXP (sym, 0)) = 0;				\
      }									\
    else if ((decl_code == REAL_CST || decl_code == STRING_CST		\
	      || decl_code == COMPLEX_CST || decl_code == CONSTRUCTOR)	\
	     && (sym = TREE_CST_RTL (decl))				\
	     && GET_CODE (sym) == MEM					\
	     && GET_CODE (XEXP (sym, 0)) == SYMBOL_REF)			\
      {									\
	SYMBOL_REF_FLAG (XEXP (sym, 0)) = 1;				\
	if (first_encoding						\
	    && current_function_decl != NULL_TREE			\
	    && DECL_SECTION_NAME (current_function_decl) != NULL_TREE)	\
	  palmos_encode_section_info (sym, current_function_decl, 1);	\
      }									\
  }									\
while (0)

/* Redo section encoding for functions, in case a function has interesting
   attributes (section or callseq) spread across several declarations.  */
#define REDO_SECTION_INFO_P(decl)  (TREE_CODE (decl) == FUNCTION_DECL)

/* Strip encodings in symbol assembler names with the following meanings:
    func	a function in the .text section (nothing to strip)
    *.LC0	perhaps a string constant in the .text section
    =code|func	a function with a __callseq__ attribute
    @sec|func	a function in the named section
    @sec|*.LC0	some kind of constant in the named section
   Note that "*@sec|.LC0" is not a valid equivalent for the last case,
   because assemble_name() understands too much about names starting
   with `*'.  But we process it anyway, because it's not obvious when
   the compiler will add `*'s.  (`|' represents the ASCII record separator
   character, 036.)  */
#define STRIP_NAME_ENCODING(VAR, NAME)					\
do									\
  {									\
    char *_name = (NAME);						\
    while (*_name == '*' || *_name == '=' || *_name == '@') {		\
      if (*_name != '*')  while (*_name != '\036')  _name++;		\
      _name++;								\
      }									\
    (VAR) = _name;							\
  }									\
while (0)


#undef ASM_OUTPUT_LABELREF
#define ASM_OUTPUT_LABELREF(file, name)					\
  do {									\
  char *real_name;							\
  STRIP_NAME_ENCODING (real_name, (name));				\
  asm_fprintf (file, "%0U%s", real_name);				\
  } while (0)


#undef LEGITIMATE_PIC_OPERAND_P
#define LEGITIMATE_PIC_OPERAND_P(X)					\
  ((! symbolic_operand (X, VOIDmode)					\
    && ! (GET_CODE (X) == CONST_DOUBLE && CONST_DOUBLE_MEM (X)		\
	  && GET_CODE (CONST_DOUBLE_MEM (X)) == MEM			\
	  && symbolic_operand (XEXP (CONST_DOUBLE_MEM (X), 0),		\
			       VOIDmode)))				\
   || (GET_CODE (X) == PLUS						\
       && GET_CODE (XEXP (X, 0)) == PC					\
       && GET_CODE (XEXP (X, 1)) == SYMBOL_REF				\
       && SYMBOL_REF_FLAG (XEXP (X, 1)))				\
   || (GET_CODE (X) == SYMBOL_REF && SYMBOL_REF_FLAG (X))		\
   || (GET_CODE (X) == MEM && GET_CODE (XEXP (X, 0)) == SYMBOL_REF	\
       && SYMBOL_REF_FLAG (XEXP (X, 0))))
