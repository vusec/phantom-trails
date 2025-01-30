#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <optional>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Taint.h"
#include "args.h"
#include "debug.h"

#include "Classification.h"
#include "Simulator.h"

#define DRAM_BASE 0x80000000
#define DRAM_SIZE 0x10000000
#define DRAM_WSIZE 8
#define DRAM_LSIZE 64

// MDS only.
#define FORBIDDEN_REGION_START 0x8000C000
#define FORBIDDEN_REGION_END 0x8000D000

#define STARTUP_TICKS 1200
#define MAX_INSTR_TICKS 1300

#define MIN_CODE_SIZE 8
#define DATA_PADDING_LEN 8

/// Global objects used by the verilated model.
bool done_reset;
bool verbose;
double cur_time = 0;
double sc_time_stamp() { return cur_time; }

/// Used for dummy tainted stores.
uint8_t dummy[8];

/// Artificially introduce new taint to prevent libAFL from reading 0 taint in
/// some edge cases.
void dummyStore(uint8_t n) {
  taint_set_label(1, &dummy[0], 1);

  switch (n) {
  case 0:
    dummy[7] = dummy[0];
    dummy[2] = dummy[0];
    break;
  case 1:
    dummy[4] = dummy[0];
    dummy[1] = dummy[0];
    break;
  case 2:
    dummy[5] = dummy[0];
    dummy[3] = dummy[0];
    break;
  case 3:
    dummy[6] = dummy[0];
    break;
  }
}

/// Everything we might want to log at each cycle.
void loggingCallback(Simulator &sim, const args_t args) {
  if (args.printCycles) {
    std::cerr << "================ CYCLE " << std::dec << sim.ticks
              << "==============\n\n";
  }
  if (args.printROB) {
    std::cerr << "\"robState\": " << sim.getRobState().to_string() << ",\n";
  }
  if (args.printLSQ) {
    std::cerr << "\"LSQ\": {\n" << sim.getLSQ().to_string() << "\n}\n";
  }
  if (args.printRegFile) {
    std::cerr << "\"REGFILE\": {\n" << sim.printRegFile() << "\n}\n";
  }
  if (args.printBIM) {
    std::string bimDiff = sim.printBimDiff();
    if (bimDiff != "")
      std::cerr << "\"BIM DIFF\": {\n" << bimDiff << "\n}\n";
  }
  if (args.printLoop) {
    std::string loopDiff = sim.printLoopDiff();
    if (loopDiff != "")
      std::cerr << "\"LOOP DIFF\": {\n" << loopDiff << "\n}\n";
  }
  if (args.printMSHR) {
    std::cerr << "\"MSHRs\": {\n" << sim.getMSHRs().to_string() << "\n}\n";
  }
}

/// Load content of a file into a buffer. Returns a pointer to the content and
/// its size in bytes.
std::pair<char *, uint64_t> loadFile(const char *fname) {
  int fd = open(fname, O_RDONLY);
  if (fd < 0) {
    TRACE("[ERROR] Could not open file %s.\n", fname);
    return {nullptr, 0};
  }
  struct stat s;
  const int status = fstat(fd, &s);
  uint64_t size = s.st_size;

  char *content =
      reinterpret_cast<char *>(mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0));

  return {content, size};
}

/// Setup code and data regions in the simulator.
uint64_t loadProgramSections(Simulator &sim, char *buf) {
  uint64_t codeSize = 0;

  // If code regions are not specified manually, everything is code.
  uint64_t start = sim.dramOffsetToAddr(sim.initSize);
  uint64_t end = sim.memory.baseAddr + sim.programSize;
  if (sim.codeRegions.size() == 0)
    sim.codeRegions.push_back(std::make_pair(start, end));

  for (const auto &region : sim.codeRegions) {
    // Init code has already been loaded
    if (region.first < sim.initSize)
      continue;
    // Load data between code sections. Everything that is not code is
    // considered data.
    if (region.first > start) {
      uint64_t offset = sim.addrToDramOffset(start) - sim.initSize;
      TRACE("Copying data from offset 0x%lx into region (0x%lx, 0x%lx)\n",
            offset, start, region.first);
      sim.addData(&buf[offset], start, region.first - start);
    }
    // Load code.
    uint64_t offset = sim.addrToDramOffset(region.first) - sim.initSize;
    TRACE("Copying code from offset 0x%lx into region (0x%lx, 0x%lx)\n", offset,
          region.first, region.second);
    sim.addCode(&buf[offset], region.first, region.second - region.first);
    codeSize += region.second - region.first;
    start = region.second;
  }
  // Load last section.
  uint64_t offset = sim.addrToDramOffset(start) - sim.initSize;
  TRACE("Copying data from offset 0x%lx into region (0x%lx, 0x%lx)\n", offset,
        start, end);
  sim.addData(&buf[offset], start, end - start);

  return codeSize;
}

int main(int argc, char **argv) {
  // Parse command-line args.
  const args_t args(argc, argv);

  // Create simulation.
  Simulator sim(DRAM_BASE, DRAM_SIZE, DRAM_WSIZE, DRAM_LSIZE, args.romPath,
                argc, argv, args.codeRegions, args.verboseSim, args.verboseISA);

  // Start by tainting all buffers (later we can re-run on specific buffers if
  // needed.)
  TaintSource curTaintSrc = TaintSource::ALL;
  while (curTaintSrc != TaintSource::UNKNOWN) {
    // -------------------------- Initialization ------------------------------
    // Go to initial state.
    sim.doWarmup();
    sim.taintAllMemory(Simulator::dataLbl);

    // Load initialization code into the simulation.
    if (args.initCodePath) {
      auto [buf, len] = loadFile(args.initCodePath);
      sim.addCode(buf, DRAM_BASE, len);
      sim.initSize = len;
    }
    // Load MDS-specific initialization too if enabled.
    if (args.mds && args.mdsInitPath) {
      auto [buf, len] = loadFile(args.mdsInitPath);
      sim.addCode(buf, DRAM_BASE + sim.initSize, len);
      sim.initSize += len;
      // If we have MDS initialization, we need to set a forbidden region.
      sim.setForbiddenRegion(FORBIDDEN_REGION_START, FORBIDDEN_REGION_END);
    }

// Put the AFL++ forkserver after the long warmup.
#ifdef __AFL_HAVE_MANUAL_CONTROL
    if (curTaintSrc == TaintSource::ALL)
      __AFL_INIT();
#endif

    dummyStore(0);

    // Load program to run.
    auto [buf, fileLen] = loadFile(args.programPath);
    sim.program = args.programPath;
    sim.programSize = sim.initSize + fileLen;

    // Initialization provided inside of the binary (normally for debugging).
    if (args.initSize > 0) {
      sim.addCode(buf, DRAM_BASE + sim.initSize, args.initSize);
      sim.initSize += args.initSize;
      buf = &buf[args.initSize];
    }
    // Load code and data sections.
    uint64_t codeSize = loadProgramSections(sim, buf);
    uint64_t nInstr = codeSize / 4; // TODO: this is a hack.
    if (args.memdumpPath.size())
      sim.memory.dumpMemory(sim.programSize, args.memdumpPath.c_str());

    // This saves the input to the output folder (if storing all inputs
    // was enabled by the user).
    fuzzInputCallback(args.programPath);

    // -------------------------- ISA Simulation ------------------------------
    // Run the architectural simulation and get architecturally-accessed memory.
    auto simResults = sim.runArchSim(nInstr * args.spikeCyclesPerInst);
    if (simResults.num_addrs == 0) {
      TRACE("[SIM] The program was discarded by architectural simulation: "
            "exiting.\n");
      return 0;
    }
    sim.archInfo = simResults;

    dummyStore(1);

    // Re-taint all code.
    sim.taintMemory(/*start=*/0, /*size=*/sim.programSize,
                    /*taint_label=*/Simulator::dataLbl);
    // Untaint architectural code.
    sim.untaintArchAddresses(simResults.insts, simResults.num_insts);
    // Untaint architectural data.
    sim.untaintArchAddresses(simResults.addrs, simResults.num_addrs);

    // TODO: this is just a hack because we don't have multiple labels, so the
    // second time around we're just tainting MDS buffers instead of memory.
    if (curTaintSrc != TaintSource::ALL and curTaintSrc != TaintSource::MEMORY)
      sim.taintAllMemory(Simulator::untaintedLbl);

    // --------------------- Cycle-Accurate Simulation -------------------------
    CrashInfo crash;
    TRACE("[SIM] Running cycle-accurate simulation for %ld instructions.\n",
          simResults.num_retired);

    while (sim.retired < simResults.num_retired + 1 and
           sim.ticks < sim.last_retired + MAX_INSTR_TICKS) {
      auto busyInstructions = sim.getBusyInstructions();
      bool stuck = sim.step();
      if (stuck)
        sim.retired = simResults.num_retired + 1;
      cur_time++;

      if (args.mds && args.mdsTaintCycle > 0 &&
          sim.ticks == args.mdsTaintCycle) {
        TRACE("[MDS] ====== Initialization Done =======");
        sim.taintMdsBuffers(Simulator::dataLbl);
      }

      completedCycleCallback(sim.ticks);
#ifdef DEBUG
      loggingCallback(sim, args);
#endif

      // Change the color of speculative instructions.
      sim.retaintICache();

      // --------------------- Detection ---------------------------
      if (not crash.taintFound) {
        // Check if the sinks are tainted.
        if (auto taintInfo = sim.checkTaintSinks())
          crash.recordTaint(sim, *taintInfo, busyInstructions);
      } else {
        // Keep track of the tainting instruction while we wait for the flush.
        crash.updateTaintInstr(sim);
        // If the tainting instruction is retiring without an exception, the
        // taint is architectural.
        if (crash.isTaintArchitectural) {
          TRACE("Taint instruction is about to commit!");
          break;
        }
      }

      // Detect any pipeline flush.
      if (sim.isFlushing()) {
        auto flushInfo = sim.getFlushInfo();
        TRACE(
            "[SIM] Pipeline flush detected: Cycle:%lu Type=%s  Cause=%s PC=%lx "
            "RobEntry=[%d][%d]\n",
            sim.ticks, print(flushInfo.type), print(flushInfo.cause),
            flushInfo.inst.pc, flushInfo.inst.robIdx, flushInfo.inst.robBank);

        dummyStore(2);

        if (crash.taintFound) {
          if (crash.recordFlush(sim, flushInfo))
            break;
          else
            TRACE("[SIM] Flush is not relevant, ignoring...\n");
        } else if (flushInfo.type == FlushType::EXCEPTION and
                   (flushInfo.cause == FlushCause::ILLEGAL_INSTRUCTION or
                    flushInfo.cause == FlushCause::FETCH_ACCESS)) {
          TRACE("[SIM] Reached an illegal instruction, exiting.\n");
          completedSimCallback();
          return 0;
        }
      }
      // --------------------- End Detection ---------------------------
    }

    TRACE("[SIM] Ended Simulation. n_retired=%lu   ticks=%lu\n", sim.retired,
          sim.ticks);

    completedSimCallback();

    // Crash if sinks were tainted at any point of the execution.
    if (crash.taintFound) {
      std::string reason = crash.classify(sim, curTaintSrc);

      /// If we're looking for MDS, we need an extra run.
      /// TODO: Do something more elegant than string matching.
      if (args.mds and reason.rfind("Meltdown_", 0) == 0) {
        curTaintSrc = getNextTaintSource(curTaintSrc);

        if (curTaintSrc != TaintSource::UNKNOWN) {
          cur_time = 0;
          sim.reset();
          TRACE("\n ================ [SIM] Starting again\n\n");
          continue;
        }
      }

      crash.abort(args.programPath, args.printCrashReport);
    } else {
      // If we didn't find anything we're done.
      break;
    }
  }

  return 0;
}
