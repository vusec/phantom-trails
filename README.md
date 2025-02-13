# Phantom Trails

Fuzzing processor simulations with taint tracking.

More information in the [Full paper](https://download.vusec.net/papers/phantom-trails_sec25.pdf) (USENIX Security '25).

## Quick Start

Clone all dependencies:

```sh
git clone git@github.com:vusec/phantom-trails.git --recursive
git submodule update --init --recursive
```

Build the instrumented simulation in a container (takes ~40min on a 48 cores/128GB RAM machine):

```sh
cd BOOM
./start.sh
# ... Build LLVM, Boom simulation etc...
(container) phantom-trails --help
```

For more information about running the detector and the fuzzer, see `BOOM/README.md`.

### Troubleshooting

- Building job gets killed before finishing
  - It is highly likely that building LLVM saturated the available memory. Try reducing the number of building and linking jobs: replace `nproc` in `BOOM/start.sh` with a lower number
- How do I kill the fuzzer?
  - Unfortunately Ctrl^C doesn't work, you might need to `killall sim-fuzzer && killall run-FuzzConfig`

## Contents

This repository includes various components:

- Our custom setup for the [BOOM core](https://github.com/riscv-boom/riscv-boom) (`BOOM/` folder)
- Our fork of the [Spike](https://github.com/riscv-software-src/riscv-isa-sim) architectural simulator (`ArchSim/` folder)
- Our instrumentation infrastructure, based on **LLVM**
- Our fuzzing infrastructure, including of our fork of [AFL++](https://github.com/AFLplusplus/AFLplusplus) (`AFL/` folder ) and [LibAFL](https://github.com/AFLplusplus/LibAFL) (`Fuzzer/LibAFL` folder)
- A collection of **RISC-V PoCs** for the BOOM core

### BOOM

The BOOM setup consists of:

- A simulation harness used for detection (`BOOM/boom-wrapper/src`)
  - `src/main/resources/csrc` contains the `main` entrypoint for the verilated sinmulation
  - `src/main/resources/vsrc` contains the Black-Box simulated DRAM
  - `src/main/scala` contains all the Configurations we use for evaluation
- The bootrom code (`BOOM/boom-wrapper/boot`)
- The initialization code (`BOOM/boom-wrapper/init`)
- A patch to add MDS-Store Buffer to BOOM (`BOOM/patches/boom-mds.patch`)
- A `Dockerfile` for building and running the instrumented simulation with all dependencies (see `BOOM/README.md`)

### Spike


To automatically infer secrets, the BOOM wrapper uses a custom
library that wraps the Spike architectural simulation.
In the `ArchSim/` folder you can find:

- The `processor.patch` applied to Spike (adds logging and early exit)
- The `ArchSim.h` file containing code to run Spike
- The `SpikeWrapper` API used to run Spike from the fuzzer code

### Instrumentation

Our bit-precise taint tracking sanitizer (BFSAN) is implemented as a patch to MSAN (`llvm/` folder) and applied when compiling the Verilated
simulation into a binary.

Check the diff of our fork to see where MSAN is modified.

### Fuzzing Infrastructure

Our fuzzing infrastructure consists of:

- AFL++ fork that adds taint coverage (`AFL/`)
  - [this commit](https://github.com/vusec/hw-fuzzing-AFL/commit/a6a2291746ceb1d3c2dce9cb8aa2a350cfd852a3) contains the relevant diff
- Fuzzing driver (`Fuzzer/`)
  - `LibAFL/`: Our fork of LibAFL with minor patches to avoid crashes/timeouts
  - `opcodes/`: Our fork of [riscv-opcodes](https://github.com/riscv/riscv-opcodes) that adds C++ and Rust headers generation
  - Instruction generator and mutator (inside `Fuzzer/src`)

### PoCs

We also provide the PoC testsuite we used to evaluate our detector on different transient execution vulnerabilities (`Samples/`), including:
  - different versions of our newly found vulnerability Spectre-LoopPredictor (`Sample/src/spectre-lp`)
  - PoCs of known vulnerabilities (`Samples/src/pocs`)


## Evaluation

### Setup

To build our standard configuration, use `BOOM/start.sh`. This will build a MediumBoom simulation instrumented with the Software feedback.

### PoCs

The minimal detectable PoCs for each variant can be found in `Samples/src/pocs`.

In the docker container, you can run each sample using:

```bash
phantom-trails run /Samples/build/bins/pocs/<POC>
```

### Fuzzing

To start a fuzzing campaign, you can run

```bash
phantom-trails fuzz
```

This will fuzz the simulation until all the bugs listed in `expected_findings.txt` are found.

To kill the fuzzer, you must run `sudo killall sim-fuzzer && sudo killall run-FuzzConfig` (unfortunately we did not find a better way),

The results are available in the `out/causes` folder. You can disassemble with

```bash
riscv64-unknown-elf-objdump -b binary -m riscv:rv64 -M no-aliases -D <BINARY>
```

### Spectre-LP

PoCs for Spectre-LP can be found in `Samples/src/spectre-lp`. In particular, you can find:

* `poc-minimal`: the simplest case found by the fuzzer
* `poc-ret`: uses nested calls to saturate the RAS and mispredict rets consecutively
* `poc-loop`: samples that uses branch misprediction instead of RET misprediction to trigger the LP

`spectre-lp/boom-disclosure/README` provides instructions to reproduce
on the Stock BOOM configuration.

### Taint as Feedback

```bash
./start.sh "Taint"
```

Will re-build the instrumentation adding the Taint feedback. The same
instructions apply for fuzzing.

### Impact of Program Generation

The `eval-patches/` folder contains a set of patches to disable fuzzing
optimizations.

### SmallBoom

Most pre-silicon fuzzers evaluate on SmallBoom.

- Build PhantomTrails in `SmallFuzzConfig`

```
phnatom-trails build --config SmallFuzzConfig
phantom-trails fuzz --config SmallFuzzConfig
```

### MDS

To evaluate PhantomTrails on MDS, you will need to:

1. Build BOOM with MDS-SB

```bash
phantom-trails build --config MDSConfig
```

2. Run MDS sample

```bash
phantom-trails run --config MDSConfig --mds /Samples/build/bins/mds-tests/mds.bin
```



### Cycle-accurate debugging

Optionally, you can print a cycle-accurate report of the simulation.

```bash
make clean
phantom trails build --debug -j<PROCS>
phantom-trails run <PROGRAM> --verbose

# You can also inspect internal buffers.
PRINT_ROB=1 PRINT_REGFILE=1 PRINT_LSQ=1 phantom-trails run <PROGRAM> --verbose

# Dump the content of memory before starting
MEMDUMP=1 phantom-trails run <PROGRAM>
# will be in <PROGRAM>.memdump

# Print total amount of taint, cycle-by-cyle.
PRINT_COVERAGE=1 phantom-trails run <PROGRAM> --logfile out.log
```
