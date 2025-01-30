#!/bin/bash

set -e

mkdir -p "${OUTPUT_DIR}"

echo "Starting container: $CONTAINER_NAME"
mkdir -p shared
# log-driver=none disabled logging out stdout (which can be a lot of data).
docker run --log-driver=none -v $(pwd)/shared:/shared \
        --mount "type=bind,source=${OUTPUT_DIR},target=/chipyard/sims/verilator/out"  \
        --mount "type=bind,source=$(pwd)/../,target=/external"  \
        --cap-add=SYS_PTRACE \
        --security-opt seccomp=unconfined \
        --security-opt apparmor=unconfined \
        --network=host  \
        -it $CONTAINER_NAME "$@"