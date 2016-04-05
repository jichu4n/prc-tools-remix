# Note that genscripts.sh strips blank lines, but converts lines
# matching ^\t$ into blank lines on output.  So we use the latter here.

cat <<EOF
OUTPUT_FORMAT ("${OUTPUT_FORMAT}")
	
${LIB_SEARCH_DIRS}
	
/* The LENGTH of coderes determines how large the .text section (which winds
   up as the final database's main 'code' resource) may be before producing
   linker errors.  Typical values on Palm OS are "about 32K" and "about 64K":
	
   32770 is the largest .text section whose start and end can be reached from
   each other via a 16 bit signed m68k jump (although only the first 32K can
   be called from other sections, that's enough because no function can start
   beyond there because it must end in rts);
	
   64720 is the largest resource allowable all the way back to Palm OS 1.0;
	
   65505 is the largest resource allowable on Palm OS 3.0 and higher.  */
	
MEMORY
{
    coderes : ORIGIN = 0x0, LENGTH = 32770
    datares : ORIGIN = 0x0, LENGTH = 32767
	
    trap_region :	 ORIGIN = 0x1000000, LENGTH = 2
    disposition_region : ORIGIN = 0x1000010, LENGTH = 6
}
	
SECTIONS
{
    .text :
    {
	*(.text)
	. = ALIGN(4);
	bhook_start = .;
	*(bhook)
	bhook_end = .;
	. = ALIGN(4);
	ehook_start = .;
	*(ehook)
	ehook_end = .;
EOF
if [ -n "${CONSTRUCTING}" ]; then cat <<EOF
	. = ALIGN(4);
	ctors_start = .;
	*(.ctors)
	ctors_end = .;
	. = ALIGN(4);
	dtors_start = .;
	*(.dtors)
	dtors_end = .;
EOF
fi
cat <<EOF
    } > coderes
    .data :
    {
	data_start = .;
	*(.data)
	*(.gcc_exc)
    } > datares
    .bss :
    {
	bss_start = .;
       	*(.bss)
       	*(COMMON)
    } > datares
    end = ALIGN( 4 );
    edata = ALIGN( 4 );
	
    .trap : { *(.trap) } > trap_region
    .disposn : { *(.disposn) } > disposition_region
}
EOF
