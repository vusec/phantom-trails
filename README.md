# Phantom Trails

Fuzzing processor simulations with taint tracking.

More information in the [Full paper]() (USENIX Security '25).

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
  - Try reducing the number of building and linking jobs: replace `nproc` in `BOOM/start.sh` with a lower number
- How do I kill the fuzzer?
  - Unfortunately Ctrl^C doesn't work, you might need to `killall sim-fuzzer && killall run-FuzzConfig`

## Contents

This repository includes:

- Setup for the [BOOM core](https://github.com/riscv-boom/riscv-boom) (`BOOM/` folder), consisting of:
  - The simulation harness used for detection (`BOOM/boom-wrapper/src`)
  - The bootrom code (`BOOM/boom-wrapper/boot`)
  - The initialization code (`BOOM/boom-wrapper/init`)
  - A patch to add MDS-Store Buffer to BOOM (`BOOM/patches/boom-mds.patch`)
  - A `Dockerfile` for building and running the instrumented simulation with all dependencies (see `BOOM/README.md`)

- Architectural simulator (`ArchSim/` folder), specifically:
  - The `processor.patch` applied to [Spike](https://github.com/riscv-software-src/riscv-isa-sim) (adds logging and early exit)
  - The `ArchSim.h` file containing code to run Spike
  - The `SpikeWrapper` API used to run Spike from the fuzzer code

- Fuzzing infrastructure, consisting of:
  - [AFL++](https://github.com/AFLplusplus/AFLplusplus) fork that adds taint coverage (`AFL/`)
    - commit `e067de09f039ef54b15dfb30481d17df9351d462` contains the relevant diff
  - Fuzzing driver (`Fuzzer/`)
    - `LibAFL/`: Our fork of [LibAFL](https://github.com/AFLplusplus/LibAFL) with minor patches to avoid crashes/timeouts
    - `opcodes/`: Our fork of [riscv-opcodes](https://github.com/riscv/riscv-opcodes) that adds C++ and Rust headers generation
    - Instruction generator and mutator (inside `Fuzzer/src`)
  - Bit-precise taint tracking sanitizer, implemented as a patch to MSAN (`llvm/` folder)
    - commit `aa6732355ac23afca3c98822354ca44ed47d31d2` contains the relevant diff

- The PoC Testsuite we used to evaluate our detection of different transient execution vulnerabilities (`Samples/`), including:
  - different versions of our newly found vulnerability Spectre-LoopPredictor (`Sample/src/pocs-new-variant`)
  - PoCs of known vulnerabilities (`Samples/src/pocs`)


## Evaluation

// TODO
