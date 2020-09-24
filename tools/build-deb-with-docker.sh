#!/bin/bash

docker_image=$1
dist=$2
arch=$3

# The version of Docker in Travis CI doesn't yet support the --quiet flag.
docker pull "$docker_image" > /dev/null

docker run \
  --rm \
  -v "$PWD":/work \
  "$docker_image" \
  "/work/tools/build-deb.sh" "$dist" "$arch"

