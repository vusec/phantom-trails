#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

#include "top.h"
#include "debug.h"

#define COMMITTING_0                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_arch_valids_0
#define COMMITTING_1                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_arch_valids_1


#define ROB_SIZE 32
#define N_ROBS 2


// ROB bank #1
#ifdef DebugConfig
#define ROB_INST(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_debug_inst
#else
#define ROB_INST(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_uopc
#endif
#define ROB_PC(x)                                                              \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_pc_lob
#define ROB_VAL(x)                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_val___05F##x
#define ROB_BUSY(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_bsy___05F##x
#define ROB_UOPC(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_uopc
#define ROB_XCPT(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_exception___05F##x
#define ROB_BR_MASK(x)                                                         \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_br_mask
#define ROB_FTQ_IDX(x)                                                         \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_ftq_idx
#define ROB_EDGE_INST(x)                                                       \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_edge_inst
#define ROB_PHYS_DEST(x)                                                       \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_pdst
#define ROB_DEST_TYPE(x)                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop___05F##x##_dst_rtype

// ROB bank #2
#ifdef DebugConfig
#define ROB_INST_2(x)                                                          \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_debug_inst
#else
#define ROB_INST_2(x)                                                          \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_uopc
#endif
#define ROB_PC_2(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_pc_lob
#define ROB_VAL_2(x)                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_val_1_##x
#define ROB_BUSY_2(x)                                                          \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_bsy_1_##x
#define ROB_UOPC_2(x)                                                          \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_uopc
#define ROB_XCPT_2(x)                                                          \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_exception_1_##x
#define ROB_BR_MASK_2(x)                                                       \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_br_mask
#define ROB_FTQ_IDX_2(x)                                                       \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_ftq_idx
#define ROB_EDGE_INST_2(x)                                                     \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_edge_inst
#define ROB_PHYS_DEST_2(x)                                                     \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_pdst
#define ROB_DEST_TYPE_2(x)                                                    \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_1_##x##_dst_rtype

// ROB general
#define ROB_HEAD                                                               \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_head
#define ROB_HEAD_LSB                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_head_lsb
#define ROB_TAIL                                                               \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_tail
#define ROB_TAIL_LSB                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_tail_lsb
#define ROB_EMPTY                                                              \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT___empty_T_1

#ifdef DebugConfig
#define ROB_HEAD_IDX                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_head_idx
#define ROB_TAIL_IDX                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_tail_idx
#else
#define ROB_HEAD_IDX                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_rob_head_idx
#define ROB_TAIL_IDX                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_rob_tail_idx
#endif

#define DEST_UNUSED 2

// Fetch target queue
#define FTQ_ENTRY(x)                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__ftq__DOT__pcs_##x
#define ROB_PC_LOB_MASK ((1ULL << 6) - 1)

// PC calculation (RoB bank #1)
#define GET_ROB_PC(top, i)                                                     \
  ((getFetchQueuePc(top, top->ROB_FTQ_IDX(i)) & ~ROB_PC_LOB_MASK) +            \
   (top->ROB_PC(i) & ROB_PC_LOB_MASK) - (top->ROB_EDGE_INST(i) * 2))
// PC calculation (RoB bank #2)
#define GET_ROB_PC_2(top, i)                                                   \
  ((getFetchQueuePc(top, top->ROB_FTQ_IDX_2(i)) & ~ROB_PC_LOB_MASK) +          \
   (top->ROB_PC_2(i) & ROB_PC_LOB_MASK) - (top->ROB_EDGE_INST_2(i) * 2))

// Committing instruction information.
#define ROB_COMMITTING_PC                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_0_pc_lob
#define ROB_COMMITTING_FTQ_IDX                                                 \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_0_ftq_idx
#define ROB_COMMITTING_EDGE_INST                                               \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_0_edge_inst

#define ROB_COMMITTING_PC_2                                                    \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_1_pc_lob
#define ROB_COMMITTING_FTQ_IDX_2                                               \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_1_ftq_idx
#define ROB_COMMITTING_EDGE_INST_2                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_1_edge_inst

// Comitting Instr (RoB bank #1)
#define GET_COMMITTING_PC(top)                                                     \
  ((getFetchQueuePc(top->rootp, top->rootp->ROB_COMMITTING_FTQ_IDX) & ~ROB_PC_LOB_MASK) +            \
   (top->rootp->ROB_COMMITTING_PC & ROB_PC_LOB_MASK) - (top->rootp->ROB_COMMITTING_EDGE_INST * 2))

// Comitting Instr (RoB bank #2)
#define GET_COMMITTING_PC_2(top)                                                   \
  ((getFetchQueuePc(top->rootp, top->rootp->ROB_COMMITTING_FTQ_IDX_2) & ~ROB_PC_LOB_MASK) +          \
   (top->rootp->ROB_COMMITTING_PC_2 & ROB_PC_LOB_MASK) - (top->rootp->ROB_COMMITTING_EDGE_INST_2 * 2))

enum MicroOpcodes {
  uopNOP = 0,
  uopLD = 1,
  uopSTA = 2,
  uopSTD = 3,
  uopLUI = 4,
  uopADDI = 5,
  uopANDI = 6,
  uopORI = 7,
  uopXORI = 8,
  uopSLTI = 9,
  uopSLTIU = 10,
  uopSLLI = 11,
  uopSRAI = 12,
  uopSRLI = 13,
  uopSLL = 14,
  uopADD = 15,
  uopSUB = 16,
  uopSLT = 17,
  uopSLTU = 18,
  uopAND = 19,
  uopOR = 20,
  uopXOR = 21,
  uopSRA = 22,
  uopSRL = 23,
  uopBEQ = 24,
  uopBNE = 25,
  uopBGE = 26,
  uopBGEU = 27,
  uopBLT = 28,
  uopBLTU = 29,
  uopCSRRW = 30,
  uopCSRRS = 31,
  uopCSRRC = 32,
  uopCSRRWI = 33,
  uopCSRRSI = 34,
  uopCSRRCI = 35,
  uopJ = 36,
  uopJAL = 37,
  uopJALR = 38,
  uopAUIPC = 39,
  uopCFLSH = 41,
  uopFENCE = 42,
  uopADDIW = 43,
  uopADDW = 44,
  uopSUBW = 45,
  uopSLLIW = 46,
  uopSLLW = 47,
  uopSRAIW = 48,
  uopSRAW = 49,
  uopSRLIW = 50,
  uopSRLW = 51,
  uopMUL = 52,
  uopMULH = 53,
  uopMULHU = 54,
  uopMULHSU = 55,
  uopMULW = 56,
  uopDIV = 57,
  uopDIVU = 58,
  uopREM = 59,
  uopREMU = 60,
  uopDIVW = 61,
  uopDIVUW = 62,
  uopREMW = 63,
  uopREMUW = 64,
  uopFENCEI = 65,
  uopAMO_AG = 67,
  uopFMV_W_X = 68,
  uopFMV_D_X = 69,
  uopFMV_X_W = 70,
  uopFMV_X_D = 71,
  uopFSGNJ_S = 72,
  uopFSGNJ_D = 73,
  uopFCVT_S_D = 74,
  uopFCVT_D_S = 75,
  uopFCVT_S_X = 76,
  uopFCVT_D_X = 77,
  uopFCVT_X_S = 78,
  uopFCVT_X_D = 79,
  uopCMPR_S = 80,
  uopCMPR_D = 81,
  uopFCLASS_S = 82,
  uopFCLASS_D = 83,
  uopFMINMAX_S = 84,
  uopFMINMAX_D = 85,
  uopFADD_S = 87,
  uopFSUB_S = 88,
  uopFMUL_S = 89,
  uopFADD_D = 90,
  uopFSUB_D = 91,
  uopFMUL_D = 92,
  uopFMADD_S = 93,
  uopFMSUB_S = 94,
  uopFNMADD_S = 95,
  uopFNMSUB_S = 96,
  uopFMADD_D = 97,
  uopFMSUB_D = 98,
  uopFNMADD_D = 99,
  uopFNMSUB_D = 100,
  uopFDIV_S = 101,
  uopFDIV_D = 102,
  uopFSQRT_S = 103,
  uopFSQRT_D = 104,
  uopWFI = 105,
  uopERET = 106,
  uopSFENCE = 107,
  uopROCC = 108,
  uopMOV = 109,
  BUBBLE = 0x4033,
  UnknownUOP
};

static const char *getUopName(const uint8_t uopc) {
  switch (uopc) {
  case 0:
    return "uopNOP";
  case 1:
    return "uopLD";
  case 2: // store address generation
    return "uopSTA";
  case 3: // store data generation
    return "uopSTD";
  case 4:
    return "uopLUI";
  case 5:
    return "uopADDI";
  case 6:
    return "uopANDI";
  case 7:
    return "uopORI";
  case 8:
    return "uopXORI";
  case 9:
    return "uopSLTI";
  case 10:
    return "uopSLTIU";
  case 11:
    return "uopSLLI";
  case 12:
    return "uopSRAI";
  case 13:
    return "uopSRLI";
  case 14:
    return "uopSLL";
  case 15:
    return "uopADD";
  case 16:
    return "uopSUB";
  case 17:
    return "uopSLT";
  case 18:
    return "uopSLTU";
  case 19:
    return "uopAND";
  case 20:
    return "uopOR";
  case 21:
    return "uopXOR";
  case 22:
    return "uopSRA";
  case 23:
    return "uopSRL";
  case 24:
    return "uopBEQ";
  case 25:
    return "uopBNE";
  case 26:
    return "uopBGE";
  case 27:
    return "uopBGEU";
  case 28:
    return "uopBLT";
  case 29:
    return "uopBLTU";
  case 30:
    return "uopCSRRW";
  case 31:
    return "uopCSRRS";
  case 32:
    return "uopCSRRC";
  case 33:
    return "uopCSRRWI";
  case 34:
    return "uopCSRRSI";
  case 35:
    return "uopCSRRCI";
  case 36:
    return "uopJ";
  case 37:
    return "uopJAL";
  case 38:
    return "uopJALR";
  case 39:
    return "uopAUIPC";
  case 41:
    return "uopCFLSH";
  case 42:
    return "uopFENCE";
  case 43:
    return "uopADDIW";
  case 44:
    return "uopADDW";
  case 45:
    return "uopSUBW";
  case 46:
    return "uopSLLIW";
  case 47:
    return "uopSLLW";
  case 48:
    return "uopSRAIW";
  case 49:
    return "uopSRAW";
  case 50:
    return "uopSRLIW";
  case 51:
    return "uopSRLW";
  case 52:
    return "uopMUL";
  case 53:
    return "uopMULH";
  case 54:
    return "uopMULHU";
  case 55:
    return "uopMULHSU";
  case 56:
    return "uopMULW";
  case 57:
    return "uopDIV";
  case 58:
    return "uopDIVU";
  case 59:
    return "uopREM";
  case 60:
    return "uopREMU";
  case 61:
    return "uopDIVW";
  case 62:
    return "uopDIVUW";
  case 63:
    return "uopREMW";
  case 64:
    return "uopREMUW";
  case 65:
    return "uopFENCEI";
  case 67: // AMO-address gen (use normal STD for datagen)
    return "uopAMO_AG";
  case 68:
    return "uopFMV_W_X";
  case 69:
    return "uopFMV_D_X";
  case 70:
    return "uopFMV_X_W";
  case 71:
    return "uopFMV_X_D";
  case 72:
    return "uopFSGNJ_S";
  case 73:
    return "uopFSGNJ_D";
  case 74:
    return "uopFCVT_S_D";
  case 75:
    return "uopFCVT_D_S";
  case 76:
    return "uopFCVT_S_X";
  case 77:
    return "uopFCVT_D_X";
  case 78:
    return "uopFCVT_X_S";
  case 79:
    return "uopFCVT_X_D";
  case 80:
    return "uopCMPR_S";
  case 81:
    return "uopCMPR_D";
  case 82:
    return "uopFCLASS_S";
  case 83:
    return "uopFCLASS_D";
  case 84:
    return "uopFMINMAX_S";
  case 85:
    return "uopFMINMAX_D";
  case 87:
    return "uopFADD_S";
  case 88:
    return "uopFSUB_S";
  case 89:
    return "uopFMUL_S";
  case 90:
    return "uopFADD_D";
  case 91:
    return "uopFSUB_D";
  case 92:
    return "uopFMUL_D";
  case 93:
    return "uopFMADD_S";
  case 94:
    return "uopFMSUB_S";
  case 95:
    return "uopFNMADD_S";
  case 96:
    return "uopFNMSUB_S";
  case 97:
    return "uopFMADD_D";
  case 98:
    return "uopFMSUB_D";
  case 99:
    return "uopFNMADD_D";
  case 100:
    return "uopFNMSUB_D";
  case 101:
    return "uopFDIV_S";
  case 102:
    return "uopFDIV_D";
  case 103:
    return "uopFSQRT_S";
  case 104:
    return "uopFSQRT_D";
  case 105: // pass uop down the CSR pipeline
    return "uopWFI";
  case 106: // pass uop down the CSR pipeline, also is ERET
    return "uopERET";
  case 107:
    return "uopSFENCE";
  case 108:
    return "uopROCC";
  case 109: // conditional mov decoded from "add rd, x0, rs2"
    return "uopMOV";
  case 0x4033:
    return "BUBBLE";
  default:
    return "UnknownUOP";
  }
}

/// Get the PC associated to a given fetch-queue index.
static const uint64_t getFetchQueuePc(const TOP *top,
                                                const uint8_t ftqIdx) {
  switch (ftqIdx) {
#define FTQ_CASE(x)                                                            \
  case x:                                                                      \
    return top->FTQ_ENTRY(x);

    FTQ_CASE(0)
    FTQ_CASE(1)
    FTQ_CASE(2)
    FTQ_CASE(3)
    FTQ_CASE(4)
    FTQ_CASE(5)
    FTQ_CASE(6)
    FTQ_CASE(7)

    FTQ_CASE(8)
    FTQ_CASE(9)
    FTQ_CASE(10)
    FTQ_CASE(11)
    FTQ_CASE(12)
    FTQ_CASE(13)
    FTQ_CASE(14)
    FTQ_CASE(15)

    FTQ_CASE(16)
    FTQ_CASE(17)
    FTQ_CASE(18)
    FTQ_CASE(19)
    FTQ_CASE(20)
    FTQ_CASE(21)
    FTQ_CASE(22)
    FTQ_CASE(23)

    FTQ_CASE(24)
    FTQ_CASE(25)
    FTQ_CASE(26)
    FTQ_CASE(27)
    FTQ_CASE(28)
    FTQ_CASE(29)
    FTQ_CASE(30)
    FTQ_CASE(31)

#undef FTQ_CASE
  }

  return 0;
}


/// Represents an entry in the ReOrder buffer.
struct RobEntry {
  // Instruction information.
  uint64_t pc = 0;
  uint32_t inst = 0;
  bool valid = false;
  bool busy = false;
  uint8_t uopcode = 0;
  bool exception;
  // Branch information.
  uint16_t brMask = 0;
  // Physical registers.
  uint64_t dst = 0;
  uint8_t dst_type = 0;
  // Which rob bank this entry belongs to.
  int robBank = 0;
  // Which is the index of this entry inside its rob bank.
  int robIdx = 0;

  std::string to_string() const {
    std::ostringstream out;
    out << "{ \"entry\": \"ROB[" << std::dec << robIdx << "][" << robBank
        << "]\", "
        << "\"µ-opcode\": \"" << getUopName(uopcode) << "\", "
        << "\"valid\": " << valid << ", "
        << "\"busy\": " << busy << ", "
        << "\"exception\": " << exception << ", "
        << "\"pc\": \"0x" << std::hex << pc << "\", "
        << "\"branchMask\": \"0x" << std::hex << brMask << "\", "
        << "\"destReg\": \"0x" << std::hex << dst << "\", "
        << "\"dst_type\": \"0x"<< std::hex << (uint64_t)dst_type << "\""
#ifdef DebugConfig
        << ", "
        << "\"disasm\": \"DASM(" << std::hex << inst << ")\" "
#endif
        << "}";
    return out.str();
  }
};

/// Represents the state of the reorder buffer.
struct RobState {
  uint64_t head = 0, head_lsb, head_idx;
  uint64_t tail = 0, tail_lsb, tail_idx;

  /// Two-issue pipeline => two RoBs.
  RobEntry entry[ROB_SIZE][N_ROBS];

  /// Read the state of the RoB from the simulation.
  void readFrom(const TOP *top) {
    head = top->ROB_HEAD;
    head_lsb = top->ROB_HEAD_LSB;
    head_idx = top->ROB_HEAD_IDX;

    tail = top->ROB_TAIL;
    tail_lsb = top->ROB_TAIL_LSB;
    tail_idx = top->ROB_TAIL_IDX;

#define READ_ROB_ENTRY(i)                                                      \
  entry[i][0] = {.pc = GET_ROB_PC(top, i),                                     \
                 .valid = (bool)top->ROB_VAL(i),                               \
                 .busy = (bool)top->ROB_BUSY(i),                               \
                 .uopcode = top->ROB_UOPC(i),                                  \
                 .exception = (bool)top->ROB_XCPT(i),                          \
                 .brMask = top->ROB_BR_MASK(i),                                \
                 .dst = top->ROB_PHYS_DEST(i),                                 \
                 .dst_type = (uint8_t)(top->ROB_DEST_TYPE(i) & 0x3),                      \
                 .robBank = 0,                                                 \
                 .robIdx = i};                                                 \
  entry[i][1] = {.pc = GET_ROB_PC_2(top, i),                                   \
                 .inst = top->ROB_INST_2(i),                                   \
                 .valid = (bool)top->ROB_VAL_2(i),                             \
                 .busy = (bool)top->ROB_BUSY_2(i),                             \
                 .uopcode = top->ROB_UOPC_2(i),                                \
                 .exception = (bool)top->ROB_XCPT_2(i),                        \
                 .brMask = top->ROB_BR_MASK_2(i),                              \
                 .dst = top->ROB_PHYS_DEST_2(i),                               \
                 .dst_type = (uint8_t)(top->ROB_DEST_TYPE_2(i) & 0x3),                    \
                 .robBank = 1,                                                 \
                 .robIdx = i};

    READ_ROB_ENTRY(0);
    READ_ROB_ENTRY(1);
    READ_ROB_ENTRY(2);
    READ_ROB_ENTRY(3);
    READ_ROB_ENTRY(4);
    READ_ROB_ENTRY(5);
    READ_ROB_ENTRY(6);
    READ_ROB_ENTRY(7);

    READ_ROB_ENTRY(8);
    READ_ROB_ENTRY(9);
    READ_ROB_ENTRY(10);
    READ_ROB_ENTRY(11);
    READ_ROB_ENTRY(12);
    READ_ROB_ENTRY(13);
    READ_ROB_ENTRY(14);
    READ_ROB_ENTRY(15);

    READ_ROB_ENTRY(16);
    READ_ROB_ENTRY(17);
    READ_ROB_ENTRY(18);
    READ_ROB_ENTRY(19);
    READ_ROB_ENTRY(20);
    READ_ROB_ENTRY(21);
    READ_ROB_ENTRY(22);
    READ_ROB_ENTRY(23);

    READ_ROB_ENTRY(24);
    READ_ROB_ENTRY(25);
    READ_ROB_ENTRY(26);
    READ_ROB_ENTRY(27);
    READ_ROB_ENTRY(28);
    READ_ROB_ENTRY(29);
    READ_ROB_ENTRY(30);
    READ_ROB_ENTRY(31);

#undef READ_ROB_ENTRY
  }

  /// Print the rob as a string (one line per entry, from head to tail).
  std::string to_string() const {
    if (isEmpty())
      return "{}";

    const uint8_t headBank = getHeadBank();

    std::ostringstream out;
    out << "{\n";
    out << "\"head\": " << head << ",\n"
        << "\"tail\": " << tail << ",\n"
        << "\"entries\": [";
    bool done = false;
    bool first = true;
    char sep = ' ';
    for (int i = head; not done; i = (i + 1) % ROB_SIZE) {
      for (int j = 0; j < N_ROBS; j++) {
        // Skip invalid entries at the head of the RoB.
        if (i == head and j < headBank)
          continue;
        // Account for situations in which the RoB is full (head == tail).
        if (i != head)
          first = false;
        // Print entry.
        out << sep << "\n" << entry[i][j].to_string();
        sep = ',';
        if (i == tail and not first) {
          done = true;
          break;
        }
      }
    }
    out << "\n]\n}";

    return out.str();
  }

  bool isEmpty() const {
    return not entry[head][0].valid and not entry[head][1].valid;
  }

  RobEntry getHead() const { return entry[head][getHeadBank()]; }
  bool isHead(const RobEntry& entry) const {
    return entry.robIdx == head and entry.robBank == getHeadBank();
  }

  uint8_t getHeadBank() const { return entry[head][0].valid ? 0 : 1; }

  /// Given two set of masks that represent, for each bit, if the corresponding
  /// RoB entry is busy, returns a vector of entries that were busy in oldMask
  /// but are not busy anymore in newMask. This makes sense only if both masks
  /// refer to the current entries of the RoB.
  using BusyMask = std::pair<uint32_t, uint32_t>;
  const std::vector<RobEntry> getDiff(const BusyMask &oldMask,
                                      const BusyMask &newMask) const {
    uint32_t firstDiff = oldMask.first & ~(newMask.first);
    uint32_t secondDiff = oldMask.second & ~(newMask.second);

    std::vector<RobEntry> diffEntries;

    for (int i = 0; i < 32; i++) {

      if (firstDiff & (1 << i))
        diffEntries.push_back(entry[i][0]);
      if (secondDiff & (1 << i))
        diffEntries.push_back(entry[i][1]);
    }

    return diffEntries;
  }

  /// Find which instruction is responsible for the taint.
  /// @param previouslyBusy  a mask representing which entries in the rob were
  ///                        busy on the last clock cycle.
  /// @param currentlyBusy   a mask representing which entries in the rob are
  ///                        currently busy.
  /// @param addr            tainted address in the register file.
  RobEntry getTaintingInstruction(const BusyMask &previouslyBusy,
                                  const BusyMask &currentlyBusy,
                                  uint64_t addr) const {
    bool done = false;
    bool first = true;

    if (isEmpty())
      return {0};

    addr -= addr % 8;

    // Find which instruction is using the tainted entry as destination.
    std::vector<std::pair<int, int>> useTaintedRegister;
    const uint8_t headBank = getHeadBank();
    for (int i = head; not done; i = (i + 1) % ROB_SIZE) {
      for (int j = 0; j < N_ROBS; j++) {
        // Skip invalid entries at the head of the RoB.
        if (i == head and j < headBank)
          continue;
        // Account for situations in which the RoB is full (head == tail).
        if (i != head)
          first = false;


        if (entry[i][j].dst * 8 == addr
            and entry[i][j].valid
            and entry[i][j].dst_type != DEST_UNUSED)
          useTaintedRegister.push_back({i, j});

        // Found the tail, exit.
        if (i == tail and not first)
          done = true;
      }
    }

    if (useTaintedRegister.size() == 0) {
      TRACE("[ERROR] Did not find the tainting instruction %x\n", addr);
      return {0};
    }

    if (useTaintedRegister.size() == 1)
      return entry[useTaintedRegister[0].first][useTaintedRegister[0].second];

    TRACE("[ERROR] More than one inst!\n");

    // If there's more than one instruction that uses the tainted entry as
    // destination, see which one got unbusied in the last clock cycle.
    auto taintInstr = getHead();
    auto &unbusy = getDiff(previouslyBusy, currentlyBusy);
    for (auto inst : unbusy)
      if (inst.dst * 8 == addr and inst.dst_type != DEST_UNUSED)
        taintInstr = inst;

    // TODO: Handle FP regfile!
    return taintInstr;
  }
};


namespace rob {
  static bool isCommitting(const TOP* top, int n) {
    return n == 0 ? top->COMMITTING_0 : top->COMMITTING_1;
  }

  /// Return a mask for each rob that indicates which entries are both valid and
  /// busy in that rob at the given cycle.
  static std::pair<uint32_t, uint32_t> getRobBusyMask(const TOP *top) {
    uint32_t rob1 = 0, rob2 = 0;

  #define GET_ROB_BUSY(i)                                                        \
    rob1 |= ((bool)top->ROB_VAL(i) && (bool)top->ROB_BUSY(i)) << i;              \
    rob2 |= ((bool)top->ROB_VAL_2(i) && (bool)top->ROB_BUSY_2(i)) << i;

    GET_ROB_BUSY(0)
    GET_ROB_BUSY(1)
    GET_ROB_BUSY(2)
    GET_ROB_BUSY(3)
    GET_ROB_BUSY(4)
    GET_ROB_BUSY(5)
    GET_ROB_BUSY(6)
    GET_ROB_BUSY(7)
    GET_ROB_BUSY(8)
    GET_ROB_BUSY(9)
    GET_ROB_BUSY(10)
    GET_ROB_BUSY(11)
    GET_ROB_BUSY(12)
    GET_ROB_BUSY(13)
    GET_ROB_BUSY(14)
    GET_ROB_BUSY(15)
    GET_ROB_BUSY(16)
    GET_ROB_BUSY(17)
    GET_ROB_BUSY(18)
    GET_ROB_BUSY(19)
    GET_ROB_BUSY(20)
    GET_ROB_BUSY(21)
    GET_ROB_BUSY(22)
    GET_ROB_BUSY(23)
    GET_ROB_BUSY(24)
    GET_ROB_BUSY(25)
    GET_ROB_BUSY(26)
    GET_ROB_BUSY(27)
    GET_ROB_BUSY(28)
    GET_ROB_BUSY(29)
    GET_ROB_BUSY(30)
    GET_ROB_BUSY(31)

  #undef GET_ROB_BUSY

    return {rob1, rob2};
  }
}
