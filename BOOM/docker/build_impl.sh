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

# Create insecure builder to enable changing personality during docker build
# (needed to disable ASLR for MSAN, see https://github.com/google/sanitizers/issues/1614)
docker buildx create --driver-opt image=moby/buildkit:master  \
                     --use --name insecure-builder \
                     --buildkitd-flags '--allow-insecure-entitlement security.insecure --allow-insecure-entitlement network.host' || echo "Using existing builder"
docker buildx use insecure-builder
# Build.
docker buildx build --load --allow security.insecure . "$@" --network=host --tag $CONTAINER_NAME
# Switch back to default builder.
#docker buildx use default
