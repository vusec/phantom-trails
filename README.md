# Phantom Trails

Fuzzing processor simulations with (software) taint tracking.

More information in the [paper](https://download.vusec.net/papers/phantom-trails_sec25.pdf) (USENIX Security '25).

## Quick Start

First, clone the repository and all its submodules.

```sh
git clone git@github.com:vusec/phantom-trails.git
cd phantom-trails
git submodule update --init --recursive
```

Next, build the container (takes ~40min on a 48 cores/128GB RAM machine)
and start a `fish` shell inside it:

```sh
cd BOOM
./start.py shell
```

> Note: this is equivalent to the `start.sh` script mentioned in the paper's
> Artifact Appendix.

## Evaluation and Usage

To run fuzzing campaigns and reproduce the results reported in our paper, you can check the [evaluation doc page](docs/eval.md).

For more advanced usage of the tool, you can check the [dedicated doc page](docs/usage.md).

## Troubleshooting

<details>

<summary>
MemorySanitizer can not mmap the shadow memory
</summary>

If you get a runtime error like:

```
FATAL: Code XXXX is out of application range. Non-PIE build?
FATAL: MemorySanitizer can not mmap the shadow memory.
[...]
```
This is caused by a known [MSAN issue with ASLR](https://github.com/google/sanitizers/issues/1614), which should be fixed by our implementation. If this still happens on your machine, you can either:
  1) reduce `vm.mmap_rnd_bits` (e.g. `sudo sysctl vm.mmap_rnd_bits=28` instead of `32`)
  2) or disable ASLR completely (e.g. `echo 0 | sudo tee /proc/sys/kernel/randomize_va_space`)

</details>


<details>

<summary>
RISCV toolchain is not found inside of the container
</summary>

If you get errors like this:

```
*** RISCV is unset. Did you source the Chipyard auto-generated env file?
```

You need to make sure `/chipyard/env.sh` is sourced (from a bash shell).

</details>

<details>

<summary>
Docker build exits prematurely
</summary>

It is highly likely that building LLVM saturated the available memory. Try reducing the number of building and linking jobs: replace `nproc` in `BOOM/start.sh` with a lower number.
</details>

<details>

<summary>
How do I kill the fuzzer?
</summary>

When running inside of the TUI, Ctrl^C might not work. You can kill the fuzzer with `killall sim-fuzzer && killall run-FuzzConfig`
</details>

## Repo Structure

```sh
.
├── AFL      # Our AFL++ fork with coverage metrics.
├── ArchSim/ # Spike (ISA simulator) + wrapper.
├── BOOM/    # Our target.
│   ├── boom-wrapper/  # C++ harness, init code, ...
│   ├── results/       # Fuzzing results are saved here.
│   ├── scripts/       # Contains `phantom-trails` script.
│   ├── shared/        # Shared with the container.
│   └── start.py       # Entrypoint script
├── docs     # Documentation.
├── Fuzzer   # libAFL fuzzer.
├── llvm     # Our LLVM fork with BFSAN.
└── Samples  # PoCs and Spectre-LP docs.

```

More details can be found in the [components doc page](docs/components.md).

