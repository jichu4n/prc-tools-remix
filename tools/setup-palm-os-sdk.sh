#!/bin/bash

if [ "$EUID" -eq 0 ]; then
  sudo=
else
  sudo='sudo'
fi

function install_palm_os_sdk() {
  echo '> Installing Palm OS SDKs'

  local palm_os_sdk_dir='/opt/palmdev'
  if [ -d "$palm_os_sdk_dir" ] && [ -n "$(ls "$palm_os_sdk_dir/")" ]; then
    echo "Found existing Palm OS SDK files at $palm_os_sdk_dir, skipping"
    return 0
  fi

  local palm_os_sdk_zip_url='https://github.com/jichu4n/palm-os-sdk/archive/master.zip'
  local temp_dir_path="$(mktemp -d)"
  local palm_os_sdk_zip="${temp_dir_path}/palm-os-sdk.zip"

  echo "> Downloading from $palm_os_sdk_zip_url"
  (set -x && curl -L -o "$palm_os_sdk_zip" "$palm_os_sdk_zip_url")

  if ! [ -f "$palm_os_sdk_zip" ]; then
    echo 'Error: Failed to download Palm OS SDKs.'
    return 1
  fi
  echo "> Installing Palm OS SDK files from $palm_os_sdk_zip to $palm_os_sdk_dir"
  (set -x && \
    $sudo unzip -q -d "$temp_dir_path" "$palm_os_sdk_zip" && \
    $sudo rm -rf "$palm_os_sdk_dir" && \
    $sudo mv "$temp_dir_path/palm-os-sdk-master" "$palm_os_sdk_dir" && \
    rm -rf "$temp_dir_path" && \
    $sudo palmdev-prep -d sdk-5r3)
}

set -e
install_palm_os_sdk

