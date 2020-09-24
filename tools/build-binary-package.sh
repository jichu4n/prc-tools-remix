#!/bin/bash

# Script to set up a Debian-based Linux environment and build a binary
# distribution package.

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
  local base_dir="$PWD"
  local dist_dir="${base_dir}/dist"

  # Build and install into temp directory.
  rm -rf "${dist_dir}"
  "$(dirname "$0")/build.sh" "${dist_dir}"

  # Reorganize files.
  cd "${dist_dir}"
  rmdir ./usr/include  # empty
  # Delete files installed by gcc, binutils etc that conflict with system
  # packages.
  rm ./usr/share/info/*
  rm -r ./usr/share/man/man7
  (
    cd ./usr/share/man/man1 &&
    for f in *; do
      if [[ "$f" != arm-palmos-* ]] && [[ "$f" != m68k-palmos-* ]]; then
        rm $f
      fi
    done
  )
  # Reinstall prc-tools's own docs.
  make -C "${base_dir}/build/doc" DESTDIR="${dist_dir}" install

  # Create binary package.
  tar cvjf '../prc-tools-2.3-bin.tar.bz2' *
}

set -ex

install_build_deps
build

