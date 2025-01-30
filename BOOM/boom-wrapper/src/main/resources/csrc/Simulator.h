#pragma once

#include <stdio.h>
#include <sys/mman.h>

#include <cstdint>
#include <cstdlib>
#include <optional>
#include <set>
#include <vector>

#include "/driver/Fuzzer/FuzzerAPI.h"
#include "SpikeWrapper.h"
#include "Taint.h"

#include "FakeDRAM.h"
#include "Flushes.h"
#include "ICache.h"
#include "LFB.h"
#include "LSQ.h"
#include "Predictors.h"
#include "RegFile.h"
#include "Rob.h"
#include "SimMem.h"

#include "debug.h"
#include "verilated.h"

// Used by the verilated model.
extern bool done_reset;
extern bool verbose;

/// Wrapper around the Verilator simulation.
struct Simulator {
public:
  static const taint_label untaintedLbl = 0;
  static const taint_label dataLbl = 1;
  static const taint_label instLbl = 2;

  TEST_HARNESS *top; // The verilated simulation
  SimMem memory;
  const char *program = nullptr;
  uint64_t programSize = 0;

  uint64_t initSize = 0;
  std::vector<std::pair<uint64_t, uint64_t>> codeRegions;

  uint64_t ticks = 0;
  uint64_t retired = 0;
  uint64_t last_retired = 0;
  uint64_t last_retired_pc = 0;
  uint64_t n_same_inst = 0;
  std::set<uint64_t> retired_pcs;

  SimResult archInfo = {};

  enum SinkType { SINK_IREGFILE, SINK_FPREGFILE, SINK_NONE };

  // Debug stuff, only populated during debugging.
  Bim bim;
  LoopPredictor loop;

public:
  Simulator(uint64_t baseAddr, uint64_t memSize, uint64_t wordSize,
            uint64_t lineSize, const char *romPath, int argc, char **argv,
            std::vector<std::pair<uint64_t, uint64_t>> codeRegions,
            bool verbose_sim = false, bool verbose_isa = false)
      : memory(baseAddr, memSize, wordSize, lineSize),
        codeRegions(codeRegions) {

    // Set the simulation's memory.
    FakeDRAM::setSimulationDRAM(&memory);

    // Initialize verilated model
    Verilated::commandArgs(argc, argv);
    // Force disable threads. Forkserver fuzzing does not support threads.
    Verilated::threadContextp()->threads(1);
    top = new TEST_HARNESS;
    // Random reset of the simulation
    srand(0x1337);
    srand48(0x1337);
    Verilated::randReset(2);

    if (verbose_sim) {
      // Set verbose flags.
      verbose = true;
      enableDebugPrints();
      Verilated::traceEverOn(true);
    } else {
      // Disable tracing in the simulation.
      Verilated::traceEverOn(false);
    }

    // Intialize spike emulator
    initSpikeSim(baseAddr, memSize, romPath, verbose_isa);
  }

  ~Simulator() {
    // Don't destroy anything (faster).
  }

  void reset() {
    ticks = 0;
    retired = 0;
    last_retired = 0;
    retired_pcs.clear();
    done_reset = false;

    initSize = 0;

    resetSpikeSim();
  }

  /// Execute one clock tick and evaluate the simulation.
  /// Return true if we're stuck on the same pc for more than 10 instructions.
  bool step() {
    top->clock = 0;
    top->eval();

    top->clock = 1;
    top->eval();

    if (isCommitting(0)) {
      retired++;
      uint64_t committed_pc = GET_COMMITTING_PC(top);
      TRACE("[SIM] Retiring instruction %ld (0x%lx)\n", retired, committed_pc);
      retired_pcs.insert(committed_pc);
      last_retired = ticks;

      if (committed_pc == last_retired_pc) {
        n_same_inst++;
      } else {
        last_retired_pc = committed_pc;
        n_same_inst = 0;
      }

      if (n_same_inst >= 10)
        return true;
    }
#ifndef SmallFuzzConfig
    if (isCommitting(1)) {
      retired++;
      uint64_t committed_pc = GET_COMMITTING_PC_2(top);
      TRACE("[SIM] Retiring instruction %ld (0x%lx)\n", retired, committed_pc);
      retired_pcs.insert(committed_pc);
      last_retired = ticks;
    }
#endif

    ticks++;
    return false;
  }

  /// Perform initial reset routine.
  void doWarmup() {
    TRACE("[SIM] Resetting CPU... ");
    // Start reset off low, so a rising edge triggers async reset.
    top->reset = 0;
    top->clock = 0;

    top->eval();
    // Reset for several cycles, to handle pipelined reset.
    top->reset = 1;
    for (int i = 0; i < 100; i++) {
      top->clock = 0;
      top->eval();

      top->clock = 1;
      top->eval();
    }
    top->reset = 0;

    // Zero-out registers.
    memset((uint8_t *)top->rootp->IREGFILE, 0, IREGFILE_SIZE);
    memset((uint8_t *)top->rootp->FPREGFILE, 0, FPREGFILE_SIZE);

    done_reset = true;
    TRACE("done\n");
  }

  // --------------------------- Tainting ------------------------------------
public:
  void taintAllMemory(taint_label lbl) { taintMemory(0, memory.memSize, lbl); }

  /// @param start is the _offset_ in DRAM, not the absolute address.
  void taintMemory(uint64_t start, uint64_t len, taint_label lbl) {
    TRACE("[SIM] Tainting 0x%lx bytes from 0x%lx... ", len, start);
    taint_set_label(lbl, (uint8_t *)&memory.data[start], len);
    TRACE("done\n");
  }

  /// @param start is the _offset_ in DRAM, not the absolute address.
  void untaintMemory(uint64_t start, uint64_t len) {
    TRACE("[SIM] Untaining 0x%lx bytes from 0x%lx... ", len, start);
    taint_set_label(0, (uint8_t *)&memory.data[start], len);
    TRACE("done\n");
  }

  struct TaintInfo {
    SinkType sink;
    uint64_t addr;
  };

  void taintMdsBuffers(taint_label lbl) {
    taintStoreBuffer(lbl);
    taintLoadBuffer(lbl);
    taintLFB(lbl);
  }

  /// Check if any of the sinks are tainted.
  /// Returns a pair of <SinkType, FirstTaintedOffset>.
  std::optional<TaintInfo> checkTaintSinks() const {
    // Check integer register file.
    auto iregFileTaint =
        checkRegFile((uint8_t *)top->rootp->IREGFILE, IREGFILE_SIZE);
    if (iregFileTaint)
      return TaintInfo{SINK_IREGFILE, iregFileTaint.value()};

    // Check floating-point register file.
    auto fpTaint =
        checkRegFile((uint8_t *)top->rootp->FPREGFILE, FPREGFILE_SIZE);
    if (fpTaint)
      return TaintInfo{SINK_FPREGFILE, fpTaint.value()};

    return std::nullopt;
  }

  void untaintICache() {
    taint_set_label(0, (uint8_t *)(top->rootp->ICACHE_WAY(0)), ICACHE_WAY_SIZE);
    taint_set_label(0, (uint8_t *)(top->rootp->ICACHE_WAY(1)), ICACHE_WAY_SIZE);
    taint_set_label(0, (uint8_t *)(top->rootp->ICACHE_WAY(2)), ICACHE_WAY_SIZE);
    taint_set_label(0, (uint8_t *)(top->rootp->ICACHE_WAY(3)), ICACHE_WAY_SIZE);
  }

  void retaintICache() {
    retaintICacheLine((uint8_t *)(top->rootp->ICACHE_WAY(0)), ICACHE_WAY_SIZE);
    retaintICacheLine((uint8_t *)(top->rootp->ICACHE_WAY(1)), ICACHE_WAY_SIZE);
    retaintICacheLine((uint8_t *)(top->rootp->ICACHE_WAY(2)), ICACHE_WAY_SIZE);
    retaintICacheLine((uint8_t *)(top->rootp->ICACHE_WAY(3)), ICACHE_WAY_SIZE);
  }

  // ------------------------- Program Load ------------------------------------
public:
  /// Copy `len` bytes from `buf` into the simulator memory at address `start`.
  /// @return the number of bytes loaded.
  uint64_t loadBytes(char *buf, uint64_t startAddr, uint64_t len) {
    TRACE("[SIM] Loading 0x%lx bytes to DRAM at addr 0x%lx... ", len,
          startAddr);
    if (not buf) {
      TRACE("Nothing to load, skipping.\n");
      return 0;
    }

    if (startAddr < memory.baseAddr ||
        startAddr >= memory.baseAddr + memory.memSize) {
      TRACE("Invalid start, skipping.\n");
      return 0;
    }
    uint64_t startOffset = addrToDramOffset(startAddr);
    uint64_t nBytes = std::min(len, memory.memSize - startOffset);
    memcpy(&memory.data[startOffset], buf, nBytes);
    TRACE("loaded 0x%lx bytes.\n", nBytes);

    return nBytes;
  }

  /// Add `len` bytes of code to the current simulation memory. Code is
  /// untainted by default.
  uint64_t addCode(char *buf, uint64_t startAddr, uint64_t len) {
    uint64_t loadedBytes = loadBytes(buf, startAddr, len);
    return loadedBytes;
  }

  /// Add `len` bytes of data to the current simulation memory. Data is
  /// tainted by default. Data is always placed after code.
  uint64_t addData(char *buf, uint64_t startAddr, uint64_t len) {
    uint64_t loadedBytes = loadBytes(buf, startAddr, len);
    taintMemory(addrToDramOffset(startAddr), len, dataLbl);
    return loadedBytes;
  }

  // --------------------------- ArchSim ------------------------------------
public:
  /// Run the program on the spike simulator, returning all the addresses
  /// visited architecturally. If no program was loaded, returns an empty set.
  SimResult runArchSim(uint64_t cycles) const {
    if (not program)
      return {};

    TRACE("[SIM] Running spike for %ld cycles...\n", cycles);
    return ::runSpikeSim(memory.data, programSize, cycles, codeRegions,
                         initSize);
  }

  void setForbiddenRegion(uint64_t start, uint64_t end) {
    ::setSpikeForbiddenRegion(start, end);
  }

  /// Untaint architecturally-accessed data.
  void untaintArchAddresses(uint64_t *addrs, uint64_t len) {
    TRACE("[SIM] Untainting arch addresses...\n");
    for (uint64_t i = 0; i < len; i++) {
      uint64_t addr = addrs[i];
      if (addr >= memory.baseAddr + memory.memSize or addr < memory.baseAddr)
        continue;
      uint64_t memIndex = addrToDramOffset(addr);
      // Align taint to 8 bytes.
      // TODO: do we still need this?
      untaintMemory(memIndex - (memIndex % 8), 8);
    }
  }

  /// Zero-out instructions that are never executed architecturally. This avoids
  /// catching "lame" cases, e.g. a load from a constant address right after a
  /// faulty load.
  // void removeNonArchInstructions(uint64_t *insts, uint64_t len) {
  //   TRACE("[SIM] Removing non-arch instructions...\n");
  //   if (len == 0)
  //     return;

  //   uint64_t curInst = 0;

  //   for (const auto& region : codeRegions) {
  //     for (uint64_t i = region.first; i < region.second; i++) {
  //       if (curInst >= len or i < insts[curInst]) {
  //         TRACE("[SIM] Removing 0x%lx\n", i);
  //         memory.data[addrToDramOffset(i)] = 0;
  //       } else if (i == insts[curInst]) {
  //         curInst++;
  //       } else
  //         assert(false && "Instructions should be ordered.\n");
  //     }
  //   }
  // }

  // --------------------------- Inspection ------------------------------------
public:
  /// Check if there's an ongoing machine clear.
  bool isFlushing() const { return uarch::isFlushing(top->rootp); }

  /// Get more information about the cause of a pipeline flush.
  FlushInfo getFlushInfo() const {
    FlushInfo info;
    info.readFrom(top->rootp);
    return info;
  }

  /// Get a snapshot of the re-order buffer.
  RobState getRobState() const {
    RobState state;
    state.readFrom(top->rootp);
    return state;
  }

  /// Get a snapshot of the load/store queues
  LSQState getLSQ() const {
    LSQState lsq;
    lsq.readFrom(top->rootp);
    return lsq;
  }

  MSHRs getMSHRs() {
    MSHRs m;
    m.readFrom(top->rootp);
    return m;
  }

  /// Get a snapshot of the register file
  std::string printRegFile() const { return regfile::printRegFile(top->rootp); }

  std::string printBimDiff() { return bim.printDiff(top->rootp); }

  /// Get a snapshot of the loop predictor
  LoopPredictor getLoopPred() const {
    LoopPredictor l;
    l.readFrom(top->rootp);
    return l;
  }

  std::string printLoopDiff() {
    LoopPredictor newLoop;
    newLoop.readFrom(top->rootp);

    std::string diff = newLoop.diff(loop);
    loop = newLoop;

    return diff;
  }

  /// Return a bitmask for each ROB bank. Each bit indicates whether the
  /// corresponding entry is both busy and valid.
  std::pair<uint32_t, uint32_t> getBusyInstructions() const {
    return rob::getRobBusyMask(top->rootp);
  }

  /// Check if port n is committing
  bool isCommitting(int n) { return rob::isCommitting(top->rootp, n); }

  // --------------------------- Private ------------------------------------
private:
  /// Given a buffer that represents a register file, returns the first address
  /// that contains taint (if any).
  std::optional<uint64_t> checkRegFile(uint8_t *data, size_t size) const {
    std::optional<uint64_t> maybeTaintAddr = std::nullopt;

    for (uint64_t i = 0; i < size; i++) {
      taint_label lbl = taint_read_label(&data[i], 1);

#ifdef MSAN
      if (lbl) {
#else
      if (lbl == dataLbl) {
#endif
        TRACE("[REGFILE] Tainted! Byte 0x%lx (entry 0x%lx) : val 0x%x\n", i,
              i / 8, data[i]);

        // In verbose mode, swipe the whole register file and report every
        // single tainted byte. In non-verbose mode, stop at first tainted
        // address.
        if (!verbose)
          return i;
        else if (not maybeTaintAddr)
          maybeTaintAddr = i;
      }
    }

    return maybeTaintAddr;
  }

  /// Whenever taint enters the ICache, we need to worry about overtainting.
  /// If we have multiple colors, we change the color of the tainted
  /// inatruction, otherwise we just untaint the instruction bytes completely.
  void retaintICacheLine(uint8_t *cacheLine, uint64_t len) {
#ifdef MSAN
    for (uint64_t i = 0; i < len; i += 4) {

      if (taint_read_label(&cacheLine[i], 4)) {
        // Taint first 6 bits. For all 4 byte instructions this reflects an
        // opcode.
        // uint32_t inst = *(uint32_t *)(&cacheLine[i]);
        // uint8_t opcodeTaint = 0;
        // if ((inst & 0b11) == 0b11) {
        //   // 16 bit instruction, not from us so just ignore it.
        // } else if ((inst & 0b11100) != 0b11100) {
        //   // 48 bit or larger instruction, also ignore this.
        // } else {
        //   // 32 bit instruction that is from us. opcode is first 7 bits.
        //   opcodeTaint = 0x7f;
        // }
        Tainting::taintBits(&cacheLine[i], 0); // RAPHAEL 0x3f);
        Tainting::taintBits(&cacheLine[i + 1], 0);
        Tainting::taintBits(&cacheLine[i + 2], 0);
        Tainting::taintBits(&cacheLine[i + 3], 0);
      }
    }
#else
    for (uint64_t i = 0; i < len; i += 4) {
      taint_label lbl = taint_read_label(&cacheLine[i], 1);
      taint_label new_lbl = ((lbl >> 1) << 1) | ((lbl & 1) << 1);
      taint_set_label(new_lbl, &cacheLine[i], 1);
    }
#endif
  }

  void taintStoreBuffer(taint_label lbl) {
#define TAINT_SB_ENTRY(N)                                                      \
  taint_set_label(lbl, (uint8_t *)&top->rootp->STQ_DATA(N), sizeof(uint64_t));

    TAINT_SB_ENTRY(0);
    TAINT_SB_ENTRY(1);
    TAINT_SB_ENTRY(2);
    TAINT_SB_ENTRY(3);
    TAINT_SB_ENTRY(4);
    TAINT_SB_ENTRY(5);
    TAINT_SB_ENTRY(6);
    TAINT_SB_ENTRY(7);
#ifndef SmallFuzzConfig
    TAINT_SB_ENTRY(8);
    TAINT_SB_ENTRY(9);
    TAINT_SB_ENTRY(10);
    TAINT_SB_ENTRY(11);
    TAINT_SB_ENTRY(12);
    TAINT_SB_ENTRY(13);
    TAINT_SB_ENTRY(14);
    TAINT_SB_ENTRY(15);
#endif
#undef TAINT_SB_ENTRY
  }

  void taintLoadBuffer(taint_label lbl) {
    // TODO.
  }

  void taintLFB(taint_label lbl) {
    // TODO.
  }

  // Addressing functions.
public:
  /// Transform an absolute address into an offset inside of the DRAM.
  uint64_t addrToDramOffset(const uint64_t addr) {
    assert(addr >= memory.baseAddr);
    return addr - memory.baseAddr;
  }

  /// Transform an offset inseide of the DRAM into an absolute address.
  uint64_t dramOffsetToAddr(const uint64_t offset) {
    return offset + memory.baseAddr;
  }
};
