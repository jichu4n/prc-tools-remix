CC = gcc
CFLAGS = -O2 -g
# -g -DDEBUG

PREFIX = /usr/local/gnu
HOST = i386-slackware-linux
TARGET-NAME = m68k-palmos-coff

EXEEXT = 
#.exe

TOOLDIR = $(PREFIX)/bin

BINUTILSDIR = ../binutils-2.7
GCCDIR = ../gcc-2.7.2.2
GDBDIR = ../gdb-4.16

GNUARCH = /cdrom/gnu

TTY = /dev/cua0

M68K_BFD = $(PREFIX)/lib/lib$(TARGET-NAME)-bfd.a

LBFD = `if [ -f $(M68K_BFD) ]; \
	then echo $(M68K_BFD); \
	else echo "-L$(PREFIX)/lib -lbfd"; fi` -liberty

LIBCDIR = libc.0.1.2
LIBMDIR = libmf.0.1.2
LIBGLIBDIR = libglib.0.1.0

PILRCDIR = pilrc1.5

# export doesn't work on make in gnuwin32 b18...
MAKEEXPORTS = PREFIX=$(PREFIX) TOOLDIR=$(TOOLDIR) \
	LIBCDIR=$(LIBCDIR) LIBMDIR=$(LIBMDIR) LIBGLIBDIR=$(LIBGLIBDIR)

all: gdbpanel/gdbpanel.prc example/pilrctst.prc

doeverything: dummy
	echo " About to try to untar the gnutools build and install them and prc-tools"
	echo " in $(PREFIX)"
	echo " If you don't have permissions to write there turn back now"
	sleep 5
	make gnuuntar
	make patch
	make gnutools
	make all
	make install

wineverything: dummy
	echo " About to try to untar the gnutools build and install them and prc-tools"
	echo " in $(PREFIX)"
	echo " If you don't have permissions to write there turn back now"
	sleep 5
	make gnuuntar
	make patch
	make winpatch
	make gnutools
	make all
	make install

install: all dummy install-man
	cd $(LIBCDIR) ; make $(MAKEEXPORTS) install
	cd $(LIBMDIR) ; make $(MAKEEXPORTS) install
	cd $(LIBGLIBDIR) ; make $(MAKEEXPORTS) install
	cp -f pilrc$(EXEEXT) $(PREFIX)/$(TARGET-NAME)/bin
	cp -f txt2bitm$(EXEEXT) $(PREFIX)/$(TARGET-NAME)/bin
	cp -f obj-res$(EXEEXT) $(PREFIX)/$(TARGET-NAME)/bin
	cp -f build-prc$(EXEEXT) $(PREFIX)/$(TARGET-NAME)/bin
	cp -f stubgen $(PREFIX)/$(TARGET-NAME)/bin
	cp -f exportlist $(PREFIX)/$(TARGET-NAME)/bin
	-cp -raf PalmOS1 $(PREFIX)/$(TARGET-NAME)/include
	-cp -raf PalmOS2 $(PREFIX)/$(TARGET-NAME)/include
	ln -snf $(PREFIX)/$(TARGET-NAME)/include/PalmOS2 $(PREFIX)/$(TARGET-NAME)/include/PalmOS
	ln -sf $(PREFIX)/$(TARGET-NAME)/bin/pilrc$(EXEEXT) $(PREFIX)/bin/pilrc$(EXEEXT)
	ln -sf $(PREFIX)/$(TARGET-NAME)/bin/txt2bitm$(EXEEXT) $(PREFIX)/bin/txt2bitm$(EXEEXT)
	ln -sf $(PREFIX)/$(TARGET-NAME)/bin/obj-res$(EXEEXT) $(PREFIX)/bin/$(TARGET-NAME)-obj-res$(EXEEXT)
	ln -sf $(PREFIX)/$(TARGET-NAME)/bin/exportlist $(PREFIX)/bin/$(TARGET-NAME)-exportlist
	ln -sf $(PREFIX)/$(TARGET-NAME)/bin/build-prc$(EXEEXT) $(PREFIX)/bin/build-prc$(EXEEXT)
	ln -sf $(PREFIX)/$(TARGET-NAME)/bin/stubgen $(PREFIX)/bin/$(TARGET-NAME)-stubgen

install-man: dummy
	cp man/* $(PREFIX)/man/man1

send: all
	echo Hit the HotSync button on the Pilot connected to $(TTY)
	pilot-xfer $(TTY) -i example/gnuhello.prc

obj-res.o: obj-res.c
	$(CC) $(CFLAGS) -I$(PREFIX)/include -c obj-res.c

obj-res$(EXEEXT): obj-res.o
	$(CC) $(CFLAGS) obj-res.o $(LBFD) -o obj-res$(EXEEXT)

build-prc$(EXEEXT): build-prc.o emit-prc.o
	$(CC) $(CFLAGS) build-prc.o emit-prc.o -o build-prc$(EXEEXT)

txt2bitm$(EXEEXT): txt2bitm.o
	$(CC) $(CFLAGS) txt2bitm.o -o txt2bitm$(EXEEXT)

$(PILRCDIR)/pilrc:
	cd $(PILRCDIR) ; make

pilrc$(EXEEXT): $(PILRCDIR)/pilrc
	mv $(PILRCDIR)/pilrc pilrc$(EXEEXT)

exportlist: dummy
	echo \#!/bin/sh > $@
	echo -n $(TOOLDIR) >> $@
	echo -n /$(TARGET-NAME)-nm \$$1 >> $@
	echo " | grep ' T ' | cut -c12- | sort -u" >> $@
	chmod +x $@

gdbpanel/gdbpanel.prc: dummy pilrc build-prc obj-res txt2bitm libc.a libm.a libGLib.a
	cd gdbpanel ; make $(MAKEEXPORTS)

example/pilrctst.prc: dummy pilrc build-prc obj-res txt2bitm libc.a libm.a libGLib.a
	cd example ; make $(MAKEEXPORTS)

libc.a: dummy exportlist obj-res build-prc libGLib.a
	cd $(LIBCDIR) ; make $(MAKEEXPORTS)
	ln -sf $(LIBCDIR)/libc.a libc.a
	ln -sf $(LIBCDIR)/libc.a libg.a
	ln -sf $(LIBCDIR)/libc.sa libc.sa
	ln -sf $(LIBCDIR)/libc.sa libg.sa

libm.a: dummy exportlist obj-res build-prc
	cd $(LIBMDIR) ; make $(MAKEEXPORTS)
	ln -sf $(LIBMDIR)/libmf.a libm.a

libGLib.a: dummy obj-res build-prc
	cd $(LIBGLIBDIR) ; make $(MAKEEXPORTS)

gnuuntar: stamp.gnuuntar

stamp.gnuuntar:
	rm -rf ../gcc-2.7.2.2 ../binutils-2.7 ../gdb-4.16
	cd .. ; tar -zxvf $(GNUARCH)/binutils-2.7.tar.gz
	cd .. ; tar -zxvf $(GNUARCH)/gcc-2.7.2.2.tar.gz
	cd .. ; tar -zxvf $(GNUARCH)/gdb-4.16.tar.gz
	touch stamp.gnuuntar

patch: stamp.patch

stamp.patch:
	cat *.palmos.diff | (cd .. ; patch -p0 )
	touch stamp.patch

winpatch: stamp.winpatch

stamp.winpatch:
	cat *.gnuwin32.diff | (cd .. ; patch -p0 )
	touch stamp.patch

gnutools: stamp.binutils stamp.gcc stamp.gdb

stamp.binutils:
	cd $(BINUTILSDIR) ; ./configure --host=$(HOST) --prefix=$(PREFIX) --target=$(TARGET-NAME)
	cd $(BINUTILSDIR) ; make
	cd $(BINUTILSDIR) ; make install
	touch $@

stamp.gcc:
	cd $(GCCDIR) ; ./configure --host=$(HOST) --prefix=$(PREFIX) --target=$(TARGET-NAME)
	cd $(GCCDIR) ; make LANGUAGES=c
	cd $(GCCDIR) ; make LANGUAGES=c install
	touch $@

stamp.gdb:
	cd $(GDBDIR) ; ./configure --host=$(HOST) --prefix=$(PREFIX) --target=$(TARGET-NAME)
	cd $(GDBDIR) ; make
	cd $(GDBDIR) ; make install
	touch $@

dummy:

clean:
	rm -f *.[oa] build-prc$(EXEEXT) obj-res$(EXEEXT) pilrc$(EXEEXT)
	rm -f txt2bitm$(EXEEXT) exportlist *.bin *.sa
	rm -f stamp.* build.* install.*
	cd $(PILRCDIR) ; make clean
	cd gdbpanel ; make clean
	cd example ; make clean
	cd $(LIBCDIR) ; make clean
	cd $(LIBMDIR) ; make clean
	cd $(LIBGLIBDIR) ; make clean
