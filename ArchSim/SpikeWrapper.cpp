#include "SpikeWrapper.h"

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <vector>

#include "ArchSim.h"

static ArchSim *sim = nullptr;
static bool debug;

extern "C" {

void initSpikeSim(uint64_t ramBase, uint64_t ramSize, const char *romPath,
                  /*bool*/ int verbose) {
  sim = new ArchSim(ramBase, ramSize, /*record_addresses=*/true, verbose,
                    /*filter=*/true);
  sim->loadRomFromFile(romPath);
  debug = verbose;
}

void resetSpikeSim() {
  sim->reset();
}

void setSpikeForbiddenRegion(uint64_t start, uint64_t end) {
  sim->setForbiddenRegion(start, end);
}

SimResult runSpikeSim(uint8_t *program, uint64_t programLen, uint64_t maxCycles,
                      std::vector<std::pair<uint64_t, uint64_t>> codeRegions, uint64_t initSize) {
  SimResult result = {0};

  // Load RAM.
  sim->loadProgram(program, programLen, codeRegions, initSize);

  // Run.
  uint64_t ticks = sim->run(maxCycles);
  result.num_retired = ticks;

  // Early exit.
  if (ticks == 0) {
    if (debug)
      std::cerr << "[ARCHSIM] Discarded\n";
    return result;
  }

  if (debug)
    std::cerr << "[ARCHSIM] ==== Executed " << std::dec << ticks << " instructions ====\n";

  // Gather accessed memory.
  auto addrs = sim->getAccessedMem();
  result.addrs = (uint64_t *)calloc(addrs.size(), sizeof(uint64_t));

  uint64_t index = 0;
  for (uint64_t addr : addrs) {
    result.addrs[index] = addr;
    index += 1;

    if (debug)
      std::cerr << "[ARCHSIM] Accessed: " << std::hex << addr << "\n";
  }
  result.num_addrs = index;

  // Gather executed instructions.
  auto insts = sim->getExecutedInsts();
  result.insts = (uint64_t *)calloc(insts.size(), sizeof(uint64_t));

  index = 0;
  for (uint64_t addr : insts) {
    result.insts[index] = addr;
    index += 1;

    if (debug)
      std::cerr << "[ARCHSIM] Executed: " << std::hex << addr << "\n";
  }
  result.num_insts = index;

  return result;
}
}
