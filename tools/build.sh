#!/bin/bash

# Generic build script for Linux environments.

cd "$(dirname "$0")/.."
base_dir="$PWD"
dest_dir="${1:-$base_dir/dist}"

# Set CC to disable warnings that prevent compilation on modern compilers. We
# have to set this as part of CC rather than CFLAGS because the latter also
# applies to the compiled version of GCC, which doesn't recognize these flags.
clang_cc_flags="-std=gnu89 -Wno-error=incompatible-function-pointer-types -Wno-error=int-conversion"
gcc_cc_flags="-std=gnu89"
cc=${CC:-gcc}
case "$($cc --version 2>&1)" in
  *"clang"*)
    cc="$cc $clang_cc_flags"
    ;;
  *"Free Software Foundation"*)
    cc="$cc $gcc_cc_flags"
    ;;
  *)
    echo "Unable to detect compiler version. Output of '$cc --version':"
    $cc --version
    exit 1
    ;;
esac

# Set CFLAGS. We use -w to suppress compiler warnings. Otherwise, the volume of
# warnings is so large when building with a modern compiler that the build will
# fail CI with an "exceeded the maximum log length" error.
#
# -fcommon is necessary when building with GCC 10.x, as the code contains
# variables with multiple definitions across files. Although we could try to
# fix the code instead, it would require better understanding of the codebase
# and it's much easier to just revert back to the old GCC behavior with
# -fcommon instead.
# See also: https://gcc.gnu.org/gcc-10/porting_to.html.
cflags="-w -O1 -fcommon -fno-strict-aliasing"

function build() {
  cd "$base_dir"
  rm -rf build && mkdir -p build && cd build

  CC="$cc" CFLAGS="$cflags" \
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

  CC="$cc" CFLAGS="$cflags" \
  make -j1
}

function install() {
  cd "$base_dir/build"
  # If directly installing into your host system (i.e. DESTDIR=''), run the
  # following command with sudo instead.
  #
  # MAKEINFO=true is a workaround for
  # https://github.com/jichu4n/prc-tools-remix/issues/4 .
  make MAKEINFO=true DESTDIR="$dest_dir" install

  cd "$dest_dir"
  rmdir ./usr/include  # empty dir
  # Delete files installed by gcc, binutils etc that conflict with host system
  # packages.
  rm ./usr/lib/*.a
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
  make -C "$base_dir/build/doc" DESTDIR="$dest_dir" install
}

set -ex

build
install

