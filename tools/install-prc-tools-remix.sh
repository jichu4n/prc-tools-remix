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

function get_prc_tools_remix_deb_url() {
  curl -sL 'https://api.github.com/repos/jichu4n/prc-tools-remix/releases/latest' | \
    grep "\"browser_download_url\": \".*prc-tools-remix.*${1}.*\.deb\"" | \
    cut -d'"' -f4
}

function install_prc_tools_remix() {
  echo '> Installing prc-tools-remix'
  if dpkg -l prc-tools-remix >/dev/null 2>&1; then
    echo 'Package prc-tools-remix already installed, skipping'
    return 0
  fi

  local prc_tools_remix_deb_url="$(get_prc_tools_remix_deb_url "$distro")"
  if [ -z "$prc_tools_remix_deb_url" ]; then
    echo "Error: Could not find prc-tools-remix deb package for distro \"$distro\""
    return 1
  elif [ "$(echo "${prc_tools_remix_deb_url}" | wc -l)" -gt 1 ]; then
    echo -e "Error: Found too many matching deb packages:\n${prc_tools_remix_deb_url}"
    return 1
  fi

  local temp_dir_path="$(mktemp -d)"

  echo "> Downloading from ${prc_tools_remix_deb_url}"
  (set -x && cd "$temp_dir_path" && curl -LO "$prc_tools_remix_deb_url")

  local prc_tools_remix_deb="$(ls "$temp_dir_path"/*.deb)"
  if [ -z "$prc_tools_remix_deb" ]; then
    echo 'Error: Failed to download prc-tools-remix deb package.'
    return 1
  fi
  echo "> Installing from $prc_tools_remix_deb"
  (set -x && $sudo dpkg -i "$prc_tools_remix_deb" && rm -rf "$temp_dir_path")
}

set -e
install_prc_tools_remix

