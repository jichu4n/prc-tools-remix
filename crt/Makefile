CC = m68k-palmos-coff-gcc
CFLAGS = -O5 -fomit-frame-pointer -fno-builtin -Wall

#INSTALL = ../install-sh
INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_DIR = /usr/local/m68k-palmos-coff/lib

INSTALL_FILES = crt0.o gcrt0.o scrt0.o libcrt.a libscrt.a

CRTLIB_OBJS = \
  single_dreloc.o multi_dreloc.o multi_free.o reloc_chain.o \
  hooks.o gdbstub.o

SCRTLIB_OBJS = \
  palmos_GLib.o \
  ssingle_dreloc.o smulti_dreloc.o smulti_free.o sreloc_chain.o \
  hooks.o decompress.o

all: $(INSTALL_FILES)

install: $(INSTALL_FILES)
	$(INSTALL) -d $(INSTALL_DIR)
	for f in $+; do \
	  echo $${f}; \
	  $(INSTALL) -m a+r $${f} $(INSTALL_DIR); \
	done

crt0.o: crt0.c

gcrt0.o: crt0.c
	$(CC) $(CFLAGS) -DDASHG -c -o $@ $<

scrt0.o: scrt0.c

hooks.o: hooks.c crt.h
gdbstub.o: gdbstub.c
palmos_GLib.o: palmos_GLib.c palmos_GLib.h

single_dreloc.o multi_dreloc.o multi_free.o reloc_chain.o: dreloc.c
	$(CC) $(CFLAGS) -c -o $@ -DL`basename $@ .o` $<

ssingle_dreloc.o smulti_dreloc.o smulti_free.o sreloc_chain.o: dreloc.c
	$(CC) $(CFLAGS) -mown-gp -c -o $@ -DL`basename $@ .o | sed -e s/^s//` $<

libcrt.a: $(CRTLIB_OBJS)
	rm -f $@
	m68k-palmos-coff-ar cr $@ $+
	m68k-palmos-coff-ranlib $@

libscrt.a: $(SCRTLIB_OBJS)
	rm -f $@
	m68k-palmos-coff-ar cr $@ $+
	m68k-palmos-coff-ranlib $@

clean:
	-rm *.o
