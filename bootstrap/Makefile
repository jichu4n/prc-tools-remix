# Makefile for rebuilding the prc-tools bootstrap Palm OS headers.
#
# Copyright 2001 by John Marshall.
# <jmarshall@acm.org>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.

# Unlike all the others, this Makefile is not autoconfiscated, nor is it
# called by the top level Makefile.  GenBootstrap is neither built nor run
# as part of building prc-tools.  Instead, the output from GenBootstrap --
# bootstrap*.h -- is used by other parts of prc-tools instead of requiring
# a Palm OS SDK.
#
# To update bootstrap*.h you need to run this makefile manually.  (Only
# maintainers need to do this.  When building prc-tools normally, the
# existing output files are used; they don't need to be regenerated.)
# Unlike the others, this Makefile assumes that prc-tools is already
# installed.  Apart from the output files bootstrap*.h, this directory is
# effectively a separate project from the rest of prc-tools.
#
# To build GenBootstrap you will need to have a Palm OS SDK installed.
# Any version 3.0 or later is suitable.
#
# GenBootstrap will run on any 3.0+ Palm OS device, and generate its output
# files as File Stream databases.  If run on the Emulator, it will also
# update the files bootstrap*.h in the current directory (i.e., within
# the source tree, not any separate build tree) if necessary.

CC = m68k-palmos-gcc
POSE = pose

all: genbootstrap.prc

genbootstrap.prc: genbootstrap
	build-prc -n GenBootstrap -c GENB genbootstrap

genbootstrap: genbootstrap.o headers.o hostio.o

genbootstrap.o: genbootstrap.c genbootstrap.h
headers.o: headers.c genbootstrap.h
hostio.o: hostio.c genbootstrap.h

headers: genbootstrap.prc
	$(POSE) -load_apps genbootstrap.prc -run_app GenBootstrap -quit_on_exit

clean:
	-rm -f *.o genbootstrap genbootstrap.prc
