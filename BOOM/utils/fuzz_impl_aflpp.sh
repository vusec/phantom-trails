#!/bin/bash

# This is called *within* the docker container to start fuzzing.

MAIN_FUZZER_CORE="$1"
if [ -z "$MAIN_FUZZER_CORE" ]
then
      echo "\$First arg must be the core id to bind to, but is: $MAIN_FUZZER_CORE"
fi

# Save stdout/stderr
exec > afl_out
exec 2> afl_out_err

echo "Starting fuzzing"
date

# Generate the input/output folders.
mkdir -p in
mkdir -p out

# Save the currently used environment variables.
# This is useful in case we later have to debug/reproduce this run.
set > out/environment

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Save the start time (as the ctime of this variable)
touch out/start_time_marker

export FUZZING_CAUSE_DIR="out/causes/"
mkdir -p "$FUZZING_CAUSE_DIR"
export COUNTER_FOLDER="out/counters/"
mkdir -p "$COUNTER_FOLDER"

set -e
printf '\x13\x00\x00\x00' > in/nop_seed

# The simulation is deterministic, so reduce calibration.
export AFL_FAST_CAL=1
export AFL_QUIET=1

# Time that we give each test case.
TIMEOUT=60000 #ms

SHARED_ARGS="-i in -o out -t $TIMEOUT -T phantom-trails"
RUN_ARGS="./run-FuzzConfig @@ /boom-wrapper/boot/bootrom.rv64.img 0 20 /boom-wrapper/init/init"

python3 "${SCRIPT_DIR}/afl_helper.py" --expected expected_findings --out out --quiet &
# Rest is gonna be killed or sigint'd.
set +e
/fuzzer/afl/afl-fuzz $SHARED_ARGS -b $MAIN_FUZZER_CORE -- $RUN_ARGS

echo "Stopped fuzzing"
date

# Cleanup all child fuzzers.
killall afl-fuzz
exit 0
