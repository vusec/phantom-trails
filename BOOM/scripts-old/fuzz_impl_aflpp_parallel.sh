#!/bin/bash

# This is called *within* the docker container to start fuzzing.

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

# Time that we give each test case.
TIMEOUT=60000 #ms

SHARED_ARGS="-i in -o out -t $TIMEOUT -T phantom-trails"
RUN_ARGS="./run-FuzzConfig @@ /boom-wrapper/boot/bootrom.rv64.img 0 20 /boom-wrapper/init/init"

WORKERS=$(nproc)
# 'seq' goes from 0 to the worker number >inclusive<, so substract one.
((WORKERS=WORKERS-1))

MAIN_FUZZER_CORE=1
echo "Main fuzzer binding to core ${MAIN_FUZZER_CORE}"

for CORE in $(seq 0 $WORKERS); do
    if [ "$MAIN_FUZZER_CORE" = "$CORE" ]; then
        continue
    fi

    echo "Starting child fuzzer (bound to core ${CORE})"
    AFL_QUIET=1 /fuzzer/afl/afl-fuzz $SHARED_ARGS -S "$CORE" -b $CORE -- $RUN_ARGS 1>/dev/null 2>/dev/null &
    sleep 0.1
done

python3 "${SCRIPT_DIR}/afl_helper.py" --expected expected_findings --out out --quiet &
# Rest is gonna be killed or sigint'd.
set +e
/fuzzer/afl/afl-fuzz $SHARED_ARGS -M main -b $MAIN_FUZZER_CORE -- $RUN_ARGS
# Cleanup all child fuzzers.
killall afl-fuzz
exit 0
