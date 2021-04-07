#!/bin/bash
# Source: https://tldp.org/REF/palmdevqs/index.html

cd "$(dirname "$0")"
set -ex

mkdir -p build && cd build
m68k-palmos-gcc ../hello.c -o hello
m68k-palmos-obj-res hello
build-prc hello.prc "Hello, World" WRLD *.hello.grc

ls -l hello.prc

