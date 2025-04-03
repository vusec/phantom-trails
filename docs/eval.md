# Evaluation

This document describes how to reproduce the results reported in the paper.

## Setup

To build the instrumented simulation in its standard configuration, use `BOOM/start.py` (e.g., `start.py shell`). By default, this will build a MediumBoom simulation instrumented with the Software feedback reported in the paper.

You can change the BOOM configuration and the coverage metric using the `--config` and `--coverage` flags, respectively.

If you already built the simulation and you just want to spawn a shell, use the `--no-rebuild` flag.

## Detected Variants

### PoCs

The minimal detectable PoCs for the main transient execution vulnerabilities known on BOOM can be found in `Samples/src/pocs`. You can check that the detector is working using `start.py test`.

To see the detector in action, you can run the detector on a single sample from within the container:

```bash
start.py shell
# ...
(container) phantom-trails run /Samples/build/bins/pocs/<POC>
```

You can also run commands inside of the container using `start.py run <COMMAND>`.

```bash
start.py run "phantom-trails run /Samples/build/bins/pocs/<POC>"
```

### Spectre-LP

PoCs for Spectre-LP can be found in [Samples/src/spectre-lp](../Samples/src/spectre-lp). In particular, you can find:

* `poc-minimal`: the simplest case found by the fuzzer
* `poc-ret`: uses nested calls to saturate the RAS and cause many consecutive `ret` mispredictions
* `poc-loop`: samples that uses branch misprediction instead of `ret` misprediction to trigger the LP

`spectre-lp/boom-disclosure/README` provides instructions to reproduce
on the Stock BOOM configuration.


### MDS

To evaluate PhantomTrails on MDS, you will need to:

1. Build BOOM with MDS-SB

```bash
./start.py shell --config MDSConfig
```

2. Run MDS sample inside of the container

```bash
(container) phantom-trails run --config MDSConfig --mds /Samples/build/bins/mds-tests/mds.bin
```


## Fuzzing

To start a fuzzing campaign, you can run (from outside the container):

```bash
start.py fuzz
```

This will build the docker container if needed, run a fuzzing campaign until all the bugs listed in `expected_findings.txt` are found, and display the TTEs and ITEs. TO hide the TUI you can use `--quiet`.

To kill the fuzzer, you might need to run `sudo killall sim-fuzzer && sudo killall run-FuzzConfig`.

All samples that triggered detection are available in the `out/causes` folder (inside the container) and in the `results/fuzzing/<CONFIG>-<timestamp>/causes` folder (outside the container).


You can disassemble specific outputs with:

```bash
riscv64-unknown-elf-objdump -b binary -m riscv:rv64 -M no-aliases -D out/causes/<BINARY>
```

### Taint as Feedback

```bash
./start.py fuzz --coverage "Taint"
```

Will re-build the instrumentation adding the Taint feedback and fuzz it. The same
instructions apply as before.

### SmallBoom

Most pre-silicon fuzzers evaluate on SmallBoom. You can run a fuzzing campaign with an instrumented SmallBoomConfig with

```bash
./start.py fuzz --config SmallBoomConfig
```

## Impact of Program Generation

Specific fuzzing optimizations can be disabled when running the fuzzer with `phantom-trails --opt`.

```bash
start.py run "phantom-trails fuzz --opt=none"
start.py run "phantom-trails fuzz --opt=basic"
start.py run "phantom-trails fuzz --opt=dataflow"
start.py run "phantom-trails fuzz --opt=controlflow"
```

Note that these are _not_ expected to find all bugs listed in `expected_findings` in a reasonable time, so you will need to manually kill the fuzzer after e.g. 24 hours and verify what bugs were discovered using `BOOM/scripts/eval-results-folder.py BOOM/results/fuzzing/<FUZZING_CAMPAIGN_FOLDER>`.
