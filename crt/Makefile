TARGETFLAGS =
DEFINES =

CC = $(TOOLDIR)/m68k-palmos-coff-gcc
AR = $(TOOLDIR)/m68k-palmos-coff-ar -cur
RANLIB = $(TOOLDIR)/m68k-palmos-coff-ranlib
NM = $(TOOLDIR)/m68k-palmos-coff-nm
INCLUDES = -I../$(LIBCDIR)/include \
-I../PalmOS1 -I../PalmOS1/UI -I../PalmOS1/System -I../PalmOS1/Hardware

CFLAGS = -O2 -g -fno-builtin $(TARGETFLAGS) $(DEFINES) $(INCLUDES)

all: crt0.o gcrt0.o scrt0.o libGLib.a

install: all
	cp -f libGLib.a $(PREFIX)/m68k-palmos-coff/lib
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/libGLib.a
	cp -f crt0.o $(PREFIX)/m68k-palmos-coff/lib
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/crt0.o
	cp -f gcrt0.o $(PREFIX)/m68k-palmos-coff/lib
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/gcrt0.o
	cp -f scrt0.o $(PREFIX)/m68k-palmos-coff/lib
	chmod 644 $(PREFIX)/m68k-palmos-coff/lib/scrt0.o

.c.s:
	$(CC) $(CFLAGS) -S $<

libGLib.a: palmos_GLib.o
	rm -f libGLib.a
	$(AR) libGLib.a palmos_GLib.o
	$(RANLIB) libGLib.a

.S.o:
	$(CC) $(TARGETFLAGS) $(DEFINES) -c $<

clean:
	rm -f *.[oa]
