
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <sys/types.h>

#include "ArchSim.h"

#define DRAM_BASE 0x80000000
#define DRAM_SIZE 0x10000000

/// Command-line args.
struct parsed_args_t {
  const char *programPath = nullptr;
  const char *romPath = nullptr;
  bool verbose = false;
  bool filter = true;
  uint64_t maxCycles = -1;
  uint64_t codeBytes = -1;

  parsed_args_t(int argc, char **argv) {
    if (argc < 3) {
      std::cerr << "Usage: " << argv[0]
                << " <BIN_PATH> <ROM_PATH> [<CYCLES>] [<VERBOSE>] [<FILTER>] "
                   "[<CODE_SIZE>] ";
      exit(-1);
    }

    // First arg is the path program to run.
    programPath = argv[1];
    // Second arg is the path of the ROM img.
    romPath = argv[2];
    // Third arg is number of cycles to run the simulation for.
    if (argc > 3)
      maxCycles = atoi(argv[3]);
    // Fourth arg is the verbosity level (default: not verbose).
    if (argc > 4)
      verbose = atoi(argv[4]);
    // Fifth arg indicates wether to apply filters or not (default: filter).
    if (argc > 5)
      filter = atoi(argv[5]);
    // Sixth arg is the number code bytes in the program (default: all bytes are
    // code).
    if (argc > 6)
      codeBytes = atoi(argv[6]);
  }
};

int main(int argc, char **argv) {
  // Parse args.
  parsed_args_t args(argc, argv);

  ArchSim sim(DRAM_BASE, DRAM_SIZE, /*record_addresses=*/true, args.verbose,
              args.filter);

  // Load ROM.
  sim.loadRomFromFile(args.romPath);
  // Load RAM.
  sim.loadProgramFromFile(args.programPath, args.codeBytes);

  // Run.
  uint64_t ticks = sim.run(args.maxCycles);

  printf("==== Executed %ld instructions ====\n", ticks);
  for (auto &addr : sim.getAccessedMem())
    printf("Accessed: %lx\n", addr);

  for (auto &addr : sim.getExecutedInsts())
    printf("Executed: %lx\n", addr);

  return 0;
}
