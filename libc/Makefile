CC = m68k-palmos-coff-gcc
CFLAGS = -Wall -O2 -fomit-frame-pointer

XOBJS = conio.o vsprintf.o printf.o ctype.o setjmp.o \
     bcopy.o memmove.o strchr.o strncat.o strspn.o \
     memscan.o strcmp.o strncmp.o strstr.o \
     memset.o strcpy.o strncpy.o strtok.o \
     memcmp.o strnlen.o strtol.o \
     memcpy.o strcat.o strlen.o strpbrk.o strtoul.o

STDLIB_OBJS = \
  div.o ldiv.o \
  malloc.o free.o calloc.o realloc.o \
  rand.o srand.o \
  strtol.o strtoul.o atoil.o \
  bmemcmp.o memcpy.o bcopy.o memset.o bzero.o

OBJS = $(STDLIB_OBJS)

GOBJS =

TARGETFLAGS =
DEFINES = -DPRINT_FLOATS

TOOLDIR = /usr/local/bin

# CC = $(TOOLDIR)/m68k-palmos-coff-gcc
AR = $(TOOLDIR)/m68k-palmos-coff-ar -cur
RANLIB = $(TOOLDIR)/m68k-palmos-coff-ranlib
NM = $(TOOLDIR)/m68k-palmos-coff-nm

BUILDPRC = ../build-prc
OBJRES = ../obj-res
STUBGEN = ../stubgen
EXPORTLIST = ../exportlist

INCLUDES = -I./include -I../PalmOS1 -I../PalmOS1/UI -I../PalmOS1/System \
-I../PalmOS1/Hardware

# CFLAGS = -O2 -fno-builtin $(TARGETFLAGS) $(DEFINES) $(INCLUDES)

all: libc.a libc.sa libg.a libg.sa

install: libc.a libc.sa libg.a libg.sa
	cp -f libc.a $(PREFIX)/m68k-palmos-coff/lib
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/libc.a
	cp -f libg.a $(PREFIX)/m68k-palmos-coff/lib
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/libg.a
	cp -f libc.sa $(PREFIX)/m68k-palmos-coff/lib/libc.sa
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/libc.sa
	cp -f libg.sa $(PREFIX)/m68k-palmos-coff/lib/libg.sa
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/libg.sa
	cp -r include/* $(PREFIX)/m68k-palmos-coff/include
	-mkdir -p $(PREFIX)/lib
	cp -f Libc.prc $(PREFIX)/lib/Libc.prc
	chmod 644 $(PREFIX)/lib/Libc.prc

.c.s:
	$(CC) $(CFLAGS) -S $<

libc.a: $(OBJS)
	rm -f libc.a
	$(AR) libc.a $(OBJS)
	$(RANLIB) libc.a

libg.a: $(OBJS) $(GOBJS)
	rm -f libg.a
	$(AR) libg.a $(OBJS) $(GOBJS)
	$(RANLIB) libg.a

libc.sa: libc.a
	rm -f libc.sa
	$(EXPORTLIST) libc.a > libc.exp
	$(STUBGEN) "C Library" Libc libcstub.c Libc.S < libc.exp
	ln -sf ../$(LIBGLIBDIR)/scrt0.o scrt0.o
	$(CC) -shared -static Libc.S ./libc.a -o Libc
	$(OBJRES) -l Libc
	$(BUILDPRC) -l Libc.prc "C Library" Libc *.Libc.grc
	$(CC) $(CFLAGS) -c libcstub.c
	$(AR) libc.sa libcstub.o
	$(RANLIB) libc.sa

libg.sa: libc.sa
	rm -f libg.sa
	ln -s libc.sa libg.sa

libgr.sa: libg.a
	rm -f libg.sa
	$(EXPORTLIST) libg.a > libg.exp
	$(STUBGEN) "Debug C Lib" Libg libgstub.c Libg.S < libg.exp
	ln -sf ../$(LIBGLIBDIR)/scrt0.o scrt0.o
	$(CC) -shared -static Libg.S ./libg.a -o Libg
	$(OBJRES) -l Libg
	$(BUILDPRC) -l Libg.prc "Debug C Lib" Libg *.Libg.grc
	$(CC) $(CFLAGS) -c libgstub.c
	$(AR) libg.sa libgstub.o
	$(RANLIB) libg.sa

.S.o:
	$(CC) $(TARGETFLAGS) $(DEFINES) -c $<

clean:
	rm -f *.[oa] Libc* libcstub* *.grc *.exp *.sa Libg* libgstub*

div.o ldiv.o: division.c
	$(CC) $(CFLAGS) -c -o $@ -DL`basename $@ .o` $<

malloc.o free.o calloc.o realloc.o: malloc.c
	$(CC) $(CFLAGS) -c -o $@ -DL`basename $@ .o` $<

rand.o srand.o: random.c
	$(CC) $(CFLAGS) -c -o $@ -DL`basename $@ .o` $<

bmemcmp.o memcpy.o bcopy.o memset.o bzero.o: memory.S
	$(CC) $(ASFLAGS) -c -o $@ -DL`basename $@ .o` $<

strtol.o strtoul.o atoil.o: strtonum.S
	$(CC) $(ASFLAGS) -c -o $@ -DL`basename $@ .o` $<
