#pragma once

#include <fstream>
#include <stdint.h>
#include <string>

#include "/driver/Fuzzer/FuzzerAPI.h"

#include "Flushes.h"
#include "Rob.h"
#include "Simulator.h"
#include "Taint.h"

#define RET_RAW_BYTES 0x00008067U
#define RET_MASK 0x000FFFFFU

typedef uint64_t insn_bits_t;
/// Extracted from the ISA decoder.
class insn_t {
public:
  insn_t() = default;
  insn_t(insn_bits_t bits) : b(bits) {}
  insn_bits_t bits() { return b; }

  int64_t sb_imm() {
    return (x(8, 4) << 1) + (x(25, 6) << 5) + (x(7, 1) << 11) +
           (imm_sign() << 12);
  }

private:
  insn_bits_t b;
  uint64_t x(int lo, int len) {
    return (b >> lo) & ((insn_bits_t(1) << len) - 1);
  }
  uint64_t xs(int lo, int len) {
    return int64_t(b) << (64 - lo - len) >> (64 - len);
  }
  uint64_t imm_sign() { return xs(31, 1); }
};

/// Contains all the information needed for deduplication/triaging.
struct CrashInfo {
  bool taintFound = false;
  uint64_t taintCycle = 0;
  RobState taintRob;
  RobEntry taintInstr;
  uint16_t taintBrMask = 0;
  bool isTaintArchitectural = false;

  bool flushFound = false;
  uint64_t flushCycle = 0;
  FlushInfo flushInfo;

  std::string reason = "Unknown";

  bool doDetailedClassification() {
    return std::getenv("DETAILED_CLASSIFICATION") != nullptr;
  }

  /// Save information about the first time the register file got tainted.
  void recordTaint(const Simulator &sim, const Simulator::TaintInfo &taintInfo,
                   RobState::BusyMask previouslyBusy) {
    TRACE("Found taint in sinks!\n");
    taintFound = true;
    taintCycle = sim.ticks;
    taintRob = sim.getRobState();
    auto currentlyBusy = sim.getBusyInstructions();
    taintInstr = taintRob.getTaintingInstruction(previouslyBusy, currentlyBusy,
                                                 taintInfo.addr);
    taintBrMask = taintInstr.brMask;
  }

  /// Track the tainting instruction during execution.
  void updateTaintInstr(const Simulator &sim) {
    if (not taintFound)
      return;

    const RobState curRob = sim.getRobState();
    const RobEntry curEntry =
        curRob.entry[taintInstr.robIdx][taintInstr.robBank];
    taintBrMask = curEntry.brMask;

    if (curRob.isHead(taintInstr) and curEntry.valid and not curEntry.busy and
        not curEntry.exception)
      isTaintArchitectural = true;
  }

  /// Check the reason of the flush and save information for later triaging.
  /// @return whether the flush was relevant or not.
  bool recordFlush(const Simulator &sim, FlushInfo &flushInfo) {
    // Ignore architectural flushes
    // TODO: Should we handle them instead?
    if (flushInfo.type == FlushType::FLUSH)
      return false;

    // Ignore branch mispredictions that are not related to the taint.
    if (flushInfo.type == FlushType::MISPREDICTION) {
      auto isKilledByBranch = [](uint64_t brMask, const uint64_t mask) {
        return (brMask & mask) != 0;
      };
      auto isExcepting = [](const RobEntry &instr, const RobState &rob) {
        return rob.entry[instr.robIdx][instr.robBank].exception;
      };

      if (not isKilledByBranch(taintBrMask, flushInfo.mispredMask)) {
        TRACE("Misprediction (mask 0x%lx) is unrelated to taint, skipping...\n",
              flushInfo.mispredMask);
        return false;
      } else if (isExcepting(taintInstr, flushInfo.robState)) {
        TRACE("Found excepting instruction nested inside a misprediction...\n");
        flushInfo.type = FlushType::MISPREDICTION;
        flushInfo.cause = FlushCause::CAUSE_MISPREDICTION_WITH_EXCEPTION;
        // TODO: keep or kill?
      }
    }

    TRACE("Flush detected!\n");
    this->flushFound = true;
    this->flushCycle = sim.ticks;
    this->flushInfo = flushInfo;
    return true;
  }

  /// Create a json report of the crash and save it on a file.
  void createReport(const char *programName) const {
    if (not taintFound) {
      TRACE("[REPORT] No taint to report, skipping...\n");
      return;
    }

    std::string crashFileName = std::string(programName) + ".crash.json";
    std::ofstream crashFile(crashFileName);

    if (crashFile.is_open()) {
      TRACE("Writing crash details to %s...\n", crashFileName.c_str());
      crashFile << "{\n";
      crashFile << "\"program\": \"" << programName << "\",\n"
                << "\"classification\": \"" << reason << "\",\n";
      // Taint information.
      crashFile << "\"taintCycle\": " << taintCycle << ",\n"
                << "\"taintInstr\": " << taintInstr.to_string() << ",\n"
                << "\"robDuringTaint\": " << taintRob.to_string() << ",\n";
      // Flush information.
      if (flushFound) {
        crashFile << "\"flushCycle\": " << flushCycle << ",\n"
                  << "\"flushInstr\": " << flushInfo.inst.to_string() << ",\n"
                  << "\"flushType\": \"" << print(flushInfo.type) << "\",\n"
                  << "\"flushCause\": \"" << print(flushInfo.cause) << "\",\n"
                  << "\"badAddr\": \"0x" << std::hex << flushInfo.badAddr
                  << "\",\n"
                  << "\"branchTag\": \"0x" << std::hex << flushInfo.mispredMask
                  << "\",\n"
                  << "\"branchTaken\": " << flushInfo.taken << ",\n"
                  << "\"robDuringFlush\": " << flushInfo.robState.to_string();
      } else {
        crashFile << "\"flushType\": \"None\"\n";
      }
      crashFile << "}\n";
      crashFile.close();
    } else {
      TRACE("[REPORT] Could not create .crash file!\n");
    }
  }

  /// Classify the bug.
  std::string classify(const Simulator &sim, TaintSource source) {
    std::string prefix;

    auto report = [&prefix, this](std::string reason) {
      this->reason = prefix + reason;
    };

    if (not taintFound)
      report("No_taint");
    else if (not flushFound)
      report("No_flush");
    else if (source != TaintSource::ALL and source != TaintSource::MEMORY) {
      prefix = "MDS_";
      report(to_string(source));
    } else if (flushInfo.type == FlushType::MISPREDICTION) {
      // if (flushInfo.cause == FlushCause::CAUSE_MISPREDICTION_WITH_EXCEPTION)
      //   prefix = "ExceptingInst_";

      switch (flushInfo.inst.uopcode) {
      case uopBEQ:
      case uopBGE:
      case uopBGEU:
      case uopBLT:
      case uopBLTU:
      case uopBNE:
        if (isStaticBranchMispred(flushInfo.inst, taintInstr.pc, sim))
          report("Spectre_v1_static");
        else if (isArchExecuted(taintInstr.pc, sim))
          report("Spectre_v1_training");
        else
          report("Spectre_v1_new");
        break;
      case uopJ:
      case uopJAL:
      case uopJALR:
        if (isRet(flushInfo.inst, sim))
          report("Spectre_RSB");
        else
          report("Spectre_v2");
        break;
      default:
        report("Spectre_Unknown");
        break;
      }
    } else if (flushInfo.type == FlushType::MEM_ORDERING) {
      // TODO: check if it's an LD-LD conflict or an LD-ST conflict.
      report("Spectre_v4");

    } else if (flushInfo.type == FlushType::EXCEPTION) {
      if (flushInfo.inst.pc == taintInstr.pc)
        prefix = "Meltdown_";
      else
        prefix = "OOO_";

      switch (flushInfo.cause) {
      case FlushCause::MISALIGNED_FETCH:
        report("MISALIGNED_FETCH");
        break;
      case FlushCause::FETCH_ACCESS:
        report("FETCH_ACCESS");
        break;
      case FlushCause::ILLEGAL_INSTRUCTION:
        report("ILLEGAL_INSTRUCTION");
        break;
      case FlushCause::BREAKPOINT:
        report("BREAKPOINT");
        break;
      case FlushCause::MISALIGNED_LOAD:
        report("MISALIGNED_LOAD");
        break;
      case FlushCause::LOAD_ACCESS:
        report("LOAD_ACCESS");
        break;
      case FlushCause::MISALIGNED_STORE:
        report("MISALIGNED_STORE");
        break;
      case FlushCause::STORE_ACCESS:
        report("STORE_ACCESS");
        break;
      case FlushCause::USER_ECALL:
        report("USER_ECALL");
        break;
      case FlushCause::SUPERVISOR_ECALL:
        report("SUPERVISOR_ECALL");
        break;
      case FlushCause::VIRTUAL_SUPERVISOR_ECALL:
        report("VSUPERVISOR_ECALL");
        break;
      case FlushCause::MACHINE_ECALL:
        report("MACHINE_ECALL");
        break;
      case FlushCause::FETCH_PAGE_FAULT:
        report("FETCH_PFAULT");
        break;
      case FlushCause::LOAD_PAGE_FAULT:
        report("LOAD_PFAULT");
        break;
      case FlushCause::STORE_PAGE_FAULT:
        report("STORE_PFAULT");
        break;
      case FlushCause::MINI_EXCEPTION_MEM_ORDERING:
        report("MEM_ORDERING");
        break;
      case FlushCause::FETCH_GUEST_PAGE_FAULT:
        report("FETCH_GUEST_PAGE_FAULT");
        break;
      case FlushCause::LOAD_GUEST_PAGE_FAULT:
        report("LOAD_GUEST_PAGE_FAULT");
        break;
      case FlushCause::VIRTUAL_INSTRUCTION:
        report("VIRTUAL_INSTRUCTION");
        break;
      case FlushCause::STORE_GUEST_PAGE_FAULT:
        report("STORE_GUEST_PAGE_FAULT");
        break;
      case FlushCause::CAUSE_UNKNOWN:
        report("UNKNOWN_CAUSE");
        break;
      default:
        report("Unknown");
      }
    } else if (flushInfo.type == FlushType::FLUSH) {
      prefix = "ArchFlush_";
      if (flushInfo.inst.pc == taintInstr.pc)
        report("SameInst");
      else
        report("DifferentInst");
    } else {
      report("Flush_Unknown");
    }

    return this->reason;
  }

  /// Abort execution.
  void abort(const char *programName, bool printReport) {
    // Find out where the fuzzer API would save the crash to.
    std::string savePath = getFuzzingSavePath(this->reason, programName);
    // If we're not fuzzing, fall back to the normal program name to save
    // the crash report next to the input file.
    if (savePath.empty())
      savePath = programName;
    if (printReport)
      createReport(savePath.c_str());

    reportFuzzingIssue(this->reason, programName);
  }

private:
  uint32_t getRawInstr(uint64_t pc, const Simulator &sim) {
    uint64_t offset = pc - sim.memory.baseAddr;
    uint32_t rawInstr = *(uint32_t *)&sim.memory.data[offset];

    return rawInstr;
  }

  bool isRet(const RobEntry &flushInstr, const Simulator &sim) {
    return (getRawInstr(flushInstr.pc, sim) & RET_MASK) == RET_RAW_BYTES;
  }

  /// Assuming that flushInstr is a branch instructions, check if taintInstrPc
  /// falls inside the "not-taken" path.
  bool isStaticBranchMispred(const RobEntry &flushInstr, uint64_t taintInstrPc,
                             const Simulator &sim) {
    // TODO: Very simplistic, a first improvement could be to check if
    // back-edges are assumed taken by the loop predictor of BOOM.
    return flushInfo.taken;
  }

  /// Check if the instruction was architecturally executed.
  bool isArchExecuted(uint64_t taintInstrPc, const Simulator &sim) {
    return sim.retired_pcs.find(taintInstrPc) != sim.retired_pcs.end();
  }
};
