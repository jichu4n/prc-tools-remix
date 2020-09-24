#!/bin/bash

# Generic build script for Linux environments.

dest_dir="$1"

cd "$(dirname "$0")/.."
rm -rf build && mkdir -p build && cd build

../prc-tools-2.3/configure \
  --enable-targets=m68k-palmos,arm-palmos \
  --enable-languages=c,c++ \
  --disable-nls \
  --build=i686-linux-gnu \
  --host=i686-linux-gnu \
  --prefix=/usr \
  --infodir=/usr/share/info \
  --mandir=/usr/share/man \
  --with-palmdev-prefix=/opt/palmdev

# We use -w to suppress compiler warnings. Otherwise, the volume of warnings
# is so large when building with a modern compiler that the build will fail
# on Travis CI with an "exceeded the maximum log length" error.
CFLAGS="-w -O2" make

# If directly installing into your host system (i.e. DESTDIR=''), run the
# following command with sudo instead.
#
# MAKEINFO=true is a workaround for
# https://github.com/jichu4n/prc-tools-remix/issues/4 .
make MAKEINFO=true DESTDIR="${dest_dir}" install

