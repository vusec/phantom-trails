# Phantom Trails Components

In this repo you will find:

- Our custom setup for the [BOOM core](https://github.com/riscv-boom/riscv-boom) (`BOOM/` folder)
- Our fork of the [Spike](https://github.com/riscv-software-src/riscv-isa-sim) architectural simulator (`ArchSim/` folder)
- Our instrumentation infrastructure, based on **LLVM** (`llvm` folder)
- Our fuzzing infrastructure, including of our fork of [AFL++](https://github.com/AFLplusplus/AFLplusplus) (`AFL/` folder ) and [LibAFL](https://github.com/AFLplusplus/LibAFL) (`Fuzzer/LibAFL` folder), and our LibAFL fuzzing driver (`Fuzzer`)
- A collection of **RISC-V PoCs** for the BOOM core (`Samples/`)

## BOOM

The BOOM setup consists of:

- A simulation harness used for detection (`BOOM/boom-wrapper/src`)
  - `src/main/resources/csrc` contains the `main` entrypoint for the verilated sinmulation
  - `src/main/resources/vsrc` contains the Black-Box simulated DRAM
  - `src/main/scala` contains all the Configurations we use for evaluation
- The bootrom code (`BOOM/boom-wrapper/boot`)
- The initialization code (`BOOM/boom-wrapper/init`)
- A patch to add MDS-Store Buffer to BOOM (`BOOM/patches/boom-mds.patch`)
- A `Dockerfile` for building and running the instrumented simulation with all dependencies (see `BOOM/README.md`)

## ArchSim

To automatically infer secrets, the BOOM wrapper uses a custom
library that wraps the Spike architectural simulation.
In the `ArchSim/` folder you can find:

- The `processor.patch` applied to Spike (adds logging and early exit)
- The `ArchSim.h` file containing code to run Spike
- The `SpikeWrapper` API used to run Spike from the fuzzer code

## LLVM Instrumentation

Our bit-precise taint tracking sanitizer (BFSAN) is implemented as a patch to MSAN (`llvm/` folder) and applied when compiling the Verilated
simulation into a binary.

Check the LLVM fork commits to see how MSAN was modified.

## Fuzzer

Our fuzzing infrastructure consists of:

- AFL++ fork that adds taint coverage (`AFL/`)
  - [this commit](https://github.com/vusec/hw-fuzzing-AFL/commit/a6a2291746ceb1d3c2dce9cb8aa2a350cfd852a3) contains the relevant diff
- Fuzzing driver (`Fuzzer/`)
  - `LibAFL/`: Our fork of LibAFL with minor patches to avoid crashes/timeouts
  - `opcodes/`: Our fork of [riscv-opcodes](https://github.com/riscv/riscv-opcodes) that adds C++ and Rust headers generation
  - Instruction generator and mutator (inside `Fuzzer/src`)

## Samples

We provide the PoC testsuite we used to evaluate our detector on different transient execution vulnerabilities (`Samples/`), including:
  - different versions of our newly found vulnerability Spectre-LoopPredictor (`Sample/src/spectre-lp`)
  - PoCs of known vulnerabilities (`Samples/src/pocs`)
