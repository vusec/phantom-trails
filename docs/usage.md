# Advanced Usage

This document describes how to use Phantom Trails beyond the scope of the paper's evaluation. If you are looking for details on fuzzing campaigns, you should check the [evaluation doc page](eval.md) first.


## `start.py`

`BOOM/start.py` provides access to some basic features (see `start.py -h`):

```sh
# Assuming in BOOM/ folder.
./start.py run "echo hello"         # Print hello and exit.
./start.py run "phantom-trails -h"  # PT script.
./start.py fuzz                     # Start fuzzing campaign.
```

The `shell` subcommand simply spawns a shell inside of a docker container. In the container, you can use the `phantom-trails` script as follows.

```sh
# Build another version of the simulation.
(container) phantom-trails build --config=<CONFIG>
# Run a test binary.
(container) phantom-trails run --config=<CONFIG> <BINARY>
# Start a fuzzing campaign (kill with `killall sim-fuzzer`).
(container) phantom-trails fuzz --config=<CONFIG>
```

## Cycle-accurate debugging

The `phantom-trails` script can also be used to inspect the cycle-accurate
behavior of a program on a given BOOM configuration. From inside the container, you can e.g.:

```sh
# Remove previous build.
make clean
# Build SmallBoom.
phantom-trails build --config SmallFuzzConfig --debug -j<PROCS>
# Run a program with verbose logging.
phantom-trails run --config SmallFuzzConfig <PROGRAM> --verbose

# You can also inspect internal buffers.
PRINT_ROB=1 PRINT_REGFILE=1 PRINT_LSQ=1 phantom-trails run <PROGRAM> --verbose --config SmallFuzzConfig

# Dump the content of memory before starting
MEMDUMP=1 phantom-trails run <PROGRAM> --config SmallFuzzConfig
# will be in <PROGRAM>.memdump

# Print total amount of taint, cycle-by-cycle.
PRINT_COVERAGE=1 phantom-trails run <PROGRAM> --logfile out.log --config SmallFuzzConfig
```

Additional printing options can be found by inspecting [args.h](BOOM/boom-wrapper/src/main/resources/csrc/args.h).
