PERL = /usr/bin/perl

SYSTRAPS_FILE = /usr/local/m68k-palmos-coff/include/PalmOS/System/SysTraps.h

#INSTALL = ../install-sh
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_DIR = /usr/local/m68k-palmos-coff/include

all: SysTraps.asm SysTraps.dat

install: MotSyntax.asm SysTraps.asm SysTraps.dat
	for f in $+; do \
	  echo $${f}; \
	  $(INSTALL_DATA) $${f} $(INSTALL_DIR); \
	done

SysTraps.asm: $(SYSTRAPS_FILE) mksystraps.PL
	$(PERL) mksystraps.PL -s $< > $@

SysTraps.dat: $(SYSTRAPS_FILE) mksystraps.PL
	$(PERL) mksystraps.PL $< > $@

clean:
	-rm SysTraps.asm SysTraps.dat
