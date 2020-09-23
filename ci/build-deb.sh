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
    --with-palmdev-prefix=/opt/palmdev \
    --host=i686-linux-gnu

  make

  $sudo make install MAKEINFO=true
}


set -ex

install_build_deps
build

