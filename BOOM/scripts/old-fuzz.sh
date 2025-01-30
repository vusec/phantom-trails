#!/bin/bash

export AFL_SKIP_CPUFREQ=1
export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1

mkdir -p in
mkdir -p out

echo "Removing all existing afl-fuzz instances"
killall afl-fuzz 2>/dev/null

# Time that we give each test case.
TIMEOUT=60000 #ms
# 24 Hour fuzz duration
FUZZ_DURATION=86400 #s

SHARED_ARGS="-i in -o out -t $TIMEOUT -V $FUZZ_DURATION -T phantom-trails"

CHILD_FUZZERS=$(nproc)
((CHILD_FUZZERS=CHILD_FUZZERS-1))

MAIN_FUZZER_CORE=1
echo "Main fuzzer binding to core ${MAIN_FUZZER_CORE}"

for CORE in $(seq 0 $CHILD_FUZZERS); do
    if [ "$MAIN_FUZZER_CORE" = "$CORE" ]; then
        continue
    fi

    echo "Starting child fuzzer (bound to core ${CORE})"
    AFL_QUIET=1 AFL_QUIET=1 /fuzzer/afl/afl-fuzz $SHARED_ARGS -S "$CORE" -b $CORE -- ./simulator-chipyard-FuzzConfig @@ 0 20 10 1>/dev/null 2>/dev/null &
    sleep 0.1
done

/fuzzer/afl/afl-fuzz $SHARED_ARGS -M main -b $MAIN_FUZZER_CORE -- ./simulator-chipyard-FuzzConfig @@ 0 20 10
# Cleanup all child fuzzers.
killall afl-fuzz
