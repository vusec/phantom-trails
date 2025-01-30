#pragma once

#include <cstdint>
#include <string>

#include "Rob.h"
#include "debug.h"
#include "top.h"

#ifndef SmallFuzzConfig
#define IS_FLUSH_ON_COMMIT                                                     \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__flush_commit
#else
#define IS_FLUSH_ON_COMMIT                                                     \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__flush_commit_mask_0
#endif

#ifdef DebugConfig
#define XCPT_VALID                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT___io_com_xcpt_valid_T_1
#define FLUSH_SIGNAL                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__flush_val
#define MISPRED_BR_MASK                                                        \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT___b1_mispredict_mask_T_4
#else
#define XCPT_VALID                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_com_xcpt_valid
#define FLUSH_SIGNAL                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_flush_valid
#define MISPRED_BR_MASK                                                        \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__dcache__DOT__mshrs__DOT__mshrs_0__DOT__rpq_io_brupdate_b1_mispredict_mask
#endif

#define XCPT_CAUSE                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__r_xcpt_uop_exc_cause
#define XCPT_BAD_ADDR                                                          \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__r_xcpt_badvaddr

// #define MISPRED_SIGNAL                                                         \
//   TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__b2_mispredict
// #define MISPRED_IDX                                                            \
//   TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__b2_uop_rob_idx

#define MISPRED_BR0_SIGNAL                                                     \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_0_mispredict
#define MISPRED_BR0_VALID                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_0_valid
#define MISPRED_BR0_ROB_IDX                                                    \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_0_uop_rob_idx
#define MISPRED_BR0_TAKEN                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_0_taken

#ifndef SmallFuzzConfig
#define MISPRED_BR1_SIGNAL                                                     \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_1_mispredict
#define MISPRED_BR1_VALID                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_1_valid
#define MISPRED_BR1_ROB_IDX                                                    \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_1_uop_rob_idx
#define MISPRED_BR1_TAKEN                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__brinfos_1_taken

#define MISPRED_USE_SECOND                                                     \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__use_this_mispredict_1;
#endif

enum class FlushType { FLUSH, EXCEPTION, MISPREDICTION, MEM_ORDERING, UNKNOWN_FLUSH, NONE };

static constexpr const char *print(const FlushType t) {
  switch (t) {
  case FlushType::FLUSH:
    return "Flush";
    break;
  case FlushType::EXCEPTION:
    return "Exception";
    break;
  case FlushType::MISPREDICTION:
    return "Misprediction";
    break;
  case FlushType::MEM_ORDERING:
    return "Memory Ordering Fault";
    break;
  case FlushType::UNKNOWN_FLUSH:
    return "Unknown flush";
    break;
  case FlushType::NONE:
    return "None";
    break;
  default:
    return "Invalid flush";
    break;
  }
}

enum class FlushCause : uint64_t {
  MISALIGNED_FETCH = 0x0,
  FETCH_ACCESS = 0x1,
  ILLEGAL_INSTRUCTION = 0x2,
  BREAKPOINT = 0x3,
  MISALIGNED_LOAD = 0x4,
  LOAD_ACCESS = 0x5,
  MISALIGNED_STORE = 0x6,
  STORE_ACCESS = 0x7,
  USER_ECALL = 0x8,
  SUPERVISOR_ECALL = 0x9,
  VIRTUAL_SUPERVISOR_ECALL = 0xa,
  MACHINE_ECALL = 0xb,
  FETCH_PAGE_FAULT = 0xc,
  LOAD_PAGE_FAULT = 0xd,
  STORE_PAGE_FAULT = 0xf,
  MINI_EXCEPTION_MEM_ORDERING = 0x10,
  FETCH_GUEST_PAGE_FAULT = 0x14,
  LOAD_GUEST_PAGE_FAULT = 0x15,
  VIRTUAL_INSTRUCTION = 0x16,
  STORE_GUEST_PAGE_FAULT = 0x17,
  CAUSE_MISPREDICTION,
  CAUSE_MISPREDICTION_WITH_EXCEPTION,
  CAUSE_UNKNOWN,
};

static constexpr const char *print(const FlushCause cause) {
  switch (cause) {
  case FlushCause::MISALIGNED_FETCH:
    return "MISALIGNED_FETCH";
  case FlushCause::FETCH_ACCESS:
    return "FETCH_ACCESS";
  case FlushCause::ILLEGAL_INSTRUCTION:
    return "ILLEGAL_INSTRUCTION";
  case FlushCause::BREAKPOINT:
    return "BREAKPOINT";
  case FlushCause::MISALIGNED_LOAD:
    return "MISALIGNED_LOAD";
  case FlushCause::LOAD_ACCESS:
    return "LOAD_ACCESS";
  case FlushCause::MISALIGNED_STORE:
    return "MISALIGNED_STORE";
  case FlushCause::STORE_ACCESS:
    return "STORE_ACCESS";
  case FlushCause::USER_ECALL:
    return "USER_ECALL";
  case FlushCause::SUPERVISOR_ECALL:
    return "SUPERVISOR_ECALL";
  case FlushCause::VIRTUAL_SUPERVISOR_ECALL:
    return "VIRTUAL_SUPERVISOR_ECALL";
  case FlushCause::MACHINE_ECALL:
    return "MACHINE_ECALL";
  case FlushCause::FETCH_PAGE_FAULT:
    return "FETCH_PAGE_FAULT";
  case FlushCause::LOAD_PAGE_FAULT:
    return "LOAD_PAGE_FAULT";
  case FlushCause::STORE_PAGE_FAULT:
    return "STORE_PAGE_FAULT";
  case FlushCause::FETCH_GUEST_PAGE_FAULT:
    return "FETCH_GUEST_PAGE_FAULT";
  case FlushCause::MINI_EXCEPTION_MEM_ORDERING:
    return "MINI_EXCEPTION_MEM_ORDERING";
  case FlushCause::LOAD_GUEST_PAGE_FAULT:
    return "LOAD_GUEST_PAGE_FAULT";
  case FlushCause::VIRTUAL_INSTRUCTION:
    return "VIRTUAL_INSTRUCTION";
  case FlushCause::STORE_GUEST_PAGE_FAULT:
    return "STORE_GUEST_PAGE_FAULT";
  case FlushCause::CAUSE_UNKNOWN:
    return "CAUSE_UNKNOWN";
  case FlushCause::CAUSE_MISPREDICTION:
    return "CAUSE_MISPREDICTION";
  case FlushCause::CAUSE_MISPREDICTION_WITH_EXCEPTION:
    return "CAUSE_MISPREDICTION_WITH_EXCEPTION";
  default:
    return "Unknown cause";
  }
}


/// Report about the flush
struct FlushInfo {
  FlushType type;
  FlushCause cause;
  RobEntry inst;
  // For memory faults, the faulting address is registered.
  uint64_t badAddr = 0x0;
  // For mispredictions, the mispredicted branch mask is recorded.
  uint64_t mispredMask = 0x0;
  RobState robState;
  // For branch misprediction, was the branch supposed to be taken.
  bool taken = false;

  void readFrom(const TOP *top) {
    robState.readFrom(top);

    if (top->FLUSH_SIGNAL) {
      // Flushes always happen when retiring the head of the RoB.
      inst = robState.getHead();

      if (top->XCPT_VALID) {
        // This flush was due to a pipeline exception.
        cause = FlushCause(top->XCPT_CAUSE);
        type = FlushType::EXCEPTION;
        badAddr = top->XCPT_BAD_ADDR;

      } else if (top->IS_FLUSH_ON_COMMIT) {
        // This flush is due to an instruction like fence, atomic mem op etc,
        // which clear the pipeline intentionally.
        type = FlushType::FLUSH;
        cause = FlushCause(top->XCPT_CAUSE);

      } else if (FlushCause(top->XCPT_CAUSE) == FlushCause::MINI_EXCEPTION_MEM_ORDERING) {
        // Mem ordering exceptions are a special case: they cause a pipeline flush,
        // but they're not a misprediction nor an exception.
        type = FlushType::MEM_ORDERING;
        cause = FlushCause(top->XCPT_CAUSE);

      } else  {
        // Exceptions or intentional flushes should be the only reasons for a
        // pipeline flush on BOOM.
        type = FlushType::UNKNOWN_FLUSH;
      }
    } else if (top->MISPRED_BR_MASK != 0) {
      cause = FlushCause::CAUSE_MISPREDICTION;
      type = FlushType::MISPREDICTION;
      mispredMask = top->MISPRED_BR_MASK;

      // For mispredictions, we need to get which instruction of the ROB actually
      // caused the misprediction.
      bool firstMispred =
          (bool)top->MISPRED_BR0_SIGNAL and (bool) top->MISPRED_BR0_VALID;

#ifndef SmallFuzzConfig
      bool secondMispred =
          (bool)top->MISPRED_BR1_SIGNAL and (bool) top->MISPRED_BR1_VALID;

      if (firstMispred or secondMispred) {
        bool useSecond = secondMispred and (bool) top->MISPRED_USE_SECOND;
        uint64_t mispredIdx =
            useSecond ? top->MISPRED_BR1_ROB_IDX : top->MISPRED_BR0_ROB_IDX;

        // Which instruction is mispredicted.
        inst = robState.entry[mispredIdx >> 1][mispredIdx & 1];
        // Was the branch supposed to be taken.
        taken = useSecond ? top->MISPRED_BR1_TAKEN : top->MISPRED_BR0_TAKEN;
#else
     if (firstMispred) {
        taken = top->MISPRED_BR0_TAKEN;
        inst = robState.entry[top->MISPRED_BR0_ROB_IDX][0];
#endif
      } else {
        TRACE("[ERROR] Misprediction mask is != 0 but no misprediction appers to "
              "be ongoing.\n");
      }
    }
  }
};


namespace uarch {
  /// Is there a pipeline flush or a misprediction happening.
  static bool isFlushing(const TOP *top) {
    return top->FLUSH_SIGNAL or top->MISPRED_BR_MASK != 0;
  }
}
