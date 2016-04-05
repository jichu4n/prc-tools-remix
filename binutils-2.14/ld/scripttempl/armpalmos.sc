cat <<EOF
OUTPUT_FORMAT ("${OUTPUT_FORMAT}", "${BIG_OUTPUT_FORMAT}",
	       "${LITTLE_OUTPUT_FORMAT}")
OUTPUT_ARCH ("${ARCH}")
	
${RELOCATING+${LIB_SEARCH_DIRS}}
	
/* This is a pathetically simple linker script that is only of use for
   building Palm OS 5 armlets, namely stand-alone code that has no global
   data or other complications.  */
	
SECTIONS
{
    .text :
    {
	*(.text .rodata)
	${RELOCATING+${OTHER_TEXT_SECTIONS}}
    }
	
    .disposn : { *(.disposn) }
}
EOF
