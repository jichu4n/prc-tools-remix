#!/bin/bash

docker_image=$1

# The version of Docker in Travis CI doesn't yet support the --quiet flag.
docker pull "$docker_image" > /dev/null

docker run \
  --rm \
  -v "$PWD":/work \
  "$docker_image" \
  "/work/ci/build-deb.sh"

