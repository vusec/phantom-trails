#!/bin/bash

export CONTAINER_NAME=boom-fuzz-manual-started

set -e

coverage_mode="${1}"
if [ -z "$coverage_mode" ]
then
      echo "No coverage mode set, assuming SW by default"
      coverage_mode="SplitCriticalEdges,BasicBlock"
fi

boom_config="${2}"
if [ -z "$boom_config" ]
then
      echo "No BOOM config set, assuming FuzzConfig (MediumBoom) by default"
      boom_config="FuzzConfig"
fi

./docker/build_impl.sh --build-arg "COVERAGE=$coverage_mode" --build-arg "BOOM_BUILD_FLAGS='--config=$boom_config --debug'" --build-arg "NPROCS=`nproc`"

export OUTPUT_DIR="$(pwd)/results/manually_started/$(date +%F-%T)"

./docker/start_impl.sh /usr/bin/fish
