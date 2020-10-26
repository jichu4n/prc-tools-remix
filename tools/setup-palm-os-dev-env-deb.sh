#!/bin/bash

prc_tools_remix_distro="${1:-focal}"

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

function get_pilrc_deb_url() {
  curl -sL 'https://api.github.com/repos/jichu4n/pilrc/releases/latest' | \
    grep "\"browser_download_url\": \".*pilrc.*${1}.*\.deb\"" | \
    cut -d'"' -f4
}

function install_prc_tools_remix() {
  echo '> Installing prc-tools-remix'
  if dpkg -l prc-tools-remix >/dev/null 2>&1; then
    echo 'Package prc-tools-remix already installed, skipping'
    return 0
  fi

  local prc_tools_remix_deb_url="$(get_prc_tools_remix_deb_url "$prc_tools_remix_distro")"
  if [ -z "$prc_tools_remix_deb_url" ]; then
    echo "Error: Could not find prc-tools-remix deb package for distro \"$prc_tools_remix_distro\""
    return 1
  elif [ "$(echo "${prc_tools_remix_deb_url}" | wc -l)" -gt 1 ]; then
    echo -e "Error: Found too many matching deb packages:\n${prc_tools_remix_deb_url}"
    return 1
  fi

  echo "> Downloading from ${prc_tools_remix_deb_url}"
  (set -x && curl -LO "$prc_tools_remix_deb_url")

  local prc_tools_remix_deb="$(ls ./*.deb)"
  if [ -z "$prc_tools_remix_deb" ]; then
    echo 'Error: Failed to download prc-tools-remix deb package.'
    return 1
  fi
  echo "> Installing from $prc_tools_remix_deb"
  (set -x && $sudo dpkg -i "$prc_tools_remix_deb" && rm "$prc_tools_remix_deb")
}

function install_palm_os_sdk() {
  echo '> Installing Palm OS SDKs'

  local palm_os_sdk_dir='/opt/palmdev'
  if [ -d "$palm_os_sdk_dir" ] && [ -n "$(ls "$palm_os_sdk_dir/")" ]; then
    echo "Found existing Palm OS SDK files at $palm_os_sdk_dir, skipping"
    return 0
  fi

  local palm_os_sdk_zip_url='https://github.com/jichu4n/palm-os-sdk/archive/master.zip'
  local palm_os_sdk_zip='palm-os-sdk.zip'

  echo "> Downloading from $palm_os_sdk_zip_url"
  (set -x && curl -L -o "$palm_os_sdk_zip" "$palm_os_sdk_zip_url")

  if ! [ -f "$palm_os_sdk_zip" ]; then
    echo 'Error: Failed to download Palm OS SDKs.'
    return 1
  fi
  echo "> Installing from $palm_os_sdk_zip"
  (set -x && \
    $sudo unzip -q "$palm_os_sdk_zip" && \
    $sudo rm -rf "$palm_os_sdk_dir" && \
    $sudo mv ./palm-os-sdk-master "$palm_os_sdk_dir" && \
    rm "$palm_os_sdk_zip")
}

function install_pilrc() {
  echo '> Installing pilrc'
  if hash pilrc >/dev/null 2>&1; then
    echo 'Found pilrc on PATH, skipping'
    return 0
  fi

  local pilrc_deb_url="$(get_pilrc_deb_url "$prc_tools_remix_distro")"
  if [ -z "$pilrc_deb_url" ]; then
    echo "Error: Could not find pilrc deb package for distro \"$prc_tools_remix_distro\""
    return 1
  elif [ "$(echo "${pilrc_deb_url}" | wc -l)" -gt 1 ]; then
    echo -e "Error: Found too many matching deb packages:\n${pilrc_deb_url}"
    return 1
  fi

  echo "> Downloading from ${pilrc_deb_url}"
  (set -x && curl -LO "$pilrc_deb_url")

  local pilrc_deb="$(ls ./*.deb)"
  if [ -z "$pilrc_deb" ]; then
    echo 'Error: Failed to download pilrc deb package.'
    return 1
  fi
  echo "> Installing from $pilrc_deb"
  (set -x && $sudo dpkg -i "$pilrc_deb" && rm "$pilrc_deb")
}

set -e

install_prc_tools_remix
echo
install_palm_os_sdk
echo
install_pilrc

