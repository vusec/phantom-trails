# Setup RISC-V BOOM for fuzzing

To build the container and open a shell inside the generated environment:

```sh
./start.sh
(container) echo "I'm in"
```

Inside the container, you can then:

```sh
# Test the detector on a specific binary.
$ phantom-trails run /Samples/build/bins/pocs/meltdown-us.bin

# Print logs from ISA and cycle-accurate simulation.
$ phantom-trails run /Samples/build/bins/pocs/meltdown-us.bin --verbose

# Fuzz the simulation from an empty seed.
$ phantom-trails fuzz
```

Use `killall sim-fuzzer; killall run-FuzzConfig` to kill the fuzzer.

Note that input binaries are expected to be just a sequence of bytes representing raw instructions (_not_ ELF files). See `/Samples`.

For more information about the available flags, see `phantom-trails --help`.

## What's happening under the hood?

The script runs `docker build` which builds a few things:

1. The instrumentation environment:
   - LLVM
   - Clang+MSAN
   - AFL++'s clang wrapper
2. The chipyard repository (pre-requisite for BOOM)
3. We add to chipyard our own generator (`FuzzConfig`) that wraps a BOOM core (the
   configurations tells chipyard how to build the chip)
4. The simulation is generated:
   - Chisel modules are gathered
   - Chisel is lowered to Verilog
   - Verilog code is turned into a C++ simulator by Verilator
   - The C++ simulator is compiled with fuzzing and tainting instrumentation
   - The final binary is produced
5. The `phantom-trails` script just wraps calling the simulation binary with different arguments
