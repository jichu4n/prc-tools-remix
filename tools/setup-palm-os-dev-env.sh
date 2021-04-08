#!/bin/bash

distro="$1"
if [ -z "$distro" ]; then
  echo "Please specify a Debian / Ubuntu distribution name, e.g. 'buster' or 'focal'."
  exit 1
fi

if [ "$EUID" -eq 0 ]; then
  sudo=
else
  sudo='sudo'
fi

set -e
cd "$(dirname "$0")"

echo
echo "Setting up Palm OS development environment for ${distro}."
echo
echo -n "Press Enter to continue..."
read

./install-prc-tools-remix.sh "$distro"
echo
./setup-palm-os-sdk.sh
echo
./install-pilrc.sh "$distro"

