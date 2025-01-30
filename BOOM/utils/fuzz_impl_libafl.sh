#!/bin/bash

# This is called *within* the docker container to start fuzzing.

# Generate the input/output folders.
mkdir -p in
mkdir -p out

# Save the currently used environment variables.
# This is useful in case we later have to debug/reproduce this run.
set > out/environment
# Get some more useful error messages from libafl...
export RUST_BACKTRACE=1

set -e
# Create a dummy seed (not actually used for now...).
echo " " > seed.txt

# Save the start time (as the ctime of this variable)
touch out/start_time_marker

# -c 1-nproc uses all cores aside from the first one. We leave that core free
# so that the libafl broker has enough CPU time. Otherwise it seems it just
# randomly crashes?
#
# All arguments after `out` are just the invocation for the fuzzer.
# @@ -> the placeholder argument that the fuzzer replaces with the path to
# the generated random program.
sim-fuzzer -c 1-`nproc` -i in -o out ./run-${1} @@ /boom-wrapper/boot/bootrom.rv64.img 20 /boom-wrapper/init/init.bin
