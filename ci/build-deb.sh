#!/bin/bash

if [ "$EUID" -eq 0 ]; then
  sudo=
else
  sudo='sudo'
fi

function install_build_deps() {
  export DEBIAN_FRONTEND=noninteractive
  $sudo apt-get -qq update
  $sudo apt-get -qq install -y \
    build-essential autoconf automake \
    libncurses-dev \
    flex bison \
    texinfo gperf \
    > /dev/null  # -qq doesn't actually silence apt-get install.
}

function build() {
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

  # We use -w  to suppress compiler warnings. Otherwise, the volume of warnings
  # is so large when building with a modern compiler that the build will fail
  # on Travis CI with an "exceeded the maximum log length" error.
  CFLAGS="-w -O2" make

  # https://github.com/jichu4n/prc-tools-remix/issues/4#issuecomment-502470945
  make MAKEINFO=true DESTDIR="$PWD/../dist" install
}

set -ex

install_build_deps
build

