#!/bin/bash

# This is called *within* the docker container to start fuzzing.
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR/.."
export AFL_DISABLE_TRIM=1
bash utils/fuzz_impl_aflpp.sh
