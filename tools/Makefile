# Makefile for prc-tools.
#
# Copyright (c) 1998 by John Marshall.
# <jmarshall@acm.org>
#
# This is free software, under the GNU General Public Licence v2 or greater.

CC = gcc
CFLAGS = -O2 -Wall -g
CPP = gcc
CPPFLAGS = -O2 -Wall -g
EXT =
LINK = $(CC) -g

.SUFFIXES: .cpp

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.cpp.o:
	$(CPP) $(CPPFLAGS) -o $@ -c $<

obj-res$(EXT): obj-res.o binres.o utils.o pfd.o
	$(LINK) -o $@ $^ -lbfd -liberty

build-prc$(EXT): build-prc.o utils.o def.yy.o def.tab.o
	$(LINK) -o $@ $^ -liberty

multigen$(EXT): multigen.o utils.o def.yy.o def.tab.o

build-prc.o: build-prc.cpp utils.h
obj-res.o: obj-res.cpp utils.h binres.hpp pfd.hpp
multigen.o: multigen.cpp utils.h
binres.o: binres.cpp binres.hpp utils.h pfd.hpp
utils.o: utils.c utils.h
pfd.o: pfd.cpp pfd.hpp

def.tab.h: def.tab.c

def.tab.c: def.y
	bison -dv $<

def.yy.c: def.l def.tab.h
	flex -is -o$@ $<

clean:
	-rm *.o def.*.* def.output
