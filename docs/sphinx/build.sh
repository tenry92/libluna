#!/usr/bin/bash

# Generate documentation using sphinx inside a temporary docker container.

# choose a name for the docker image to create
IMAGE_NAME=tenry/libluna-sphinx

# clear previously generated files
rm -rf build/html

# build docker image
docker build -t $IMAGE_NAME .

# create temporary container and generate html
docker run --rm -u $UID -v $PWD:/docs tenry/luna-game-engine-sphinx make html
