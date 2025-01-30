#!/bin/bash

set -e

echo " • Syncing AFL"
rsync -ap --exclude .git ../AFL .

echo " • Syncing LLVM"
rsync -ap --exclude .git ../llvm .

echo " • Syncing ArchSim"
rsync -ap --exclude .git ../ArchSim .

echo " • Syncing Fuzzer"
rsync -ap --exclude .git ../Fuzzer .

echo " • Syncing Samples"
rsync -ap --exclude .git --exclude build ../Samples .

echo " • Build args are: $@"

docker build . "$@" --network=host --tag $CONTAINER_NAME
