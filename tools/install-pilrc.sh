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

function get_pilrc_deb_url() {
  curl -sL 'https://api.github.com/repos/jichu4n/pilrc/releases/latest' | \
    grep "\"browser_download_url\": \".*pilrc.*${1}.*\.deb\"" | \
    cut -d'"' -f4
}

function install_pilrc() {
  echo '> Installing pilrc'
  if hash pilrc >/dev/null 2>&1; then
    echo 'Found pilrc on PATH, skipping'
    return 0
  fi

  local pilrc_deb_url="$(get_pilrc_deb_url "$distro")"
  if [ -z "$pilrc_deb_url" ]; then
    echo "Error: Could not find pilrc deb package for distro \"$distro\""
    return 1
  elif [ "$(echo "${pilrc_deb_url}" | wc -l)" -gt 1 ]; then
    echo -e "Error: Found too many matching deb packages:\n${pilrc_deb_url}"
    return 1
  fi

  local temp_dir_path="$(mktemp -d)"

  echo "> Downloading from ${pilrc_deb_url}"
  (set -x && cd "$temp_dir_path" && curl -LO "$pilrc_deb_url")

  local pilrc_deb="$(ls "$temp_dir_path"/*.deb)"
  if [ -z "$pilrc_deb" ]; then
    echo 'Error: Failed to download pilrc deb package.'
    return 1
  fi
  echo "> Installing from $pilrc_deb"
  (set -x && $sudo dpkg -i "$pilrc_deb" && rm -rf "$temp_dir_path")
}

set -e
install_pilrc

