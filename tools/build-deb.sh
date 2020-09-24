#!/bin/bash

# Script to set up a Debian-based Linux environment and build a binary
# distribution package.

dist="$1"
arch="$2"

cd "$(dirname "$0")/.."
base_dir="$PWD"
package_root_dir="$base_dir/dist"

version="$(cat "$base_dir/VERSION")"
deb_file_name="prc-tools-remix_${version}~${dist}_${arch}.deb"

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
  # Build and install into temp directory.
  rm -rf "$package_root_dir"
  "$base_dir/tools/build.sh" "$package_root_dir"
}

function package() {
  cd "$package_root_dir"
  mkdir -p DEBIAN
  cat <<EOF > DEBIAN/control
Package: prc-tools-remix
Version: $version
Section: devel
Priority: optional
Architecture: $arch
Depends:
Maintainer: Chuan Ji <chuan@jichu4n.com>
Description: Collection of tools supporting C and C++ programming for Palm OS
 prc-tools is a collection of tools supporting C and C++ programming for Palm
 OS. It supplies a full GCC cross-compilation toolchain for m68k and ARM, as
 well as Palm OS-specific utilities.
EOF

  cd "$base_dir"
  dpkg-deb --build "$package_root_dir" "$deb_file_name"
}

set -ex

install_build_deps
build
package

