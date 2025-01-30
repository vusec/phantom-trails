#pragma once

#define COMMITTING_0                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_arch_valids_0

#define ROB_SIZE 32
#define N_ROBS 1

// ROB bank #1
#ifdef DebugConfig
#define ROB_INST(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_debug_inst
#else
#define ROB_INST(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_uopc
#endif
#define ROB_PC(x)                                                              \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_pc_lob
#define ROB_VAL(x)                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_val_##x
#define ROB_BUSY(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_bsy_##x
#define ROB_UOPC(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_uopc
#define ROB_XCPT(x)                                                            \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_exception_##x
#define ROB_BR_MASK(x)                                                         \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_br_mask
#define ROB_FTQ_IDX(x)                                                         \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_ftq_idx
#define ROB_EDGE_INST(x)                                                       \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_edge_inst
#define ROB_PHYS_DEST(x)                                                       \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_pdst
#define ROB_DEST_TYPE(x)                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_uop_##x##_dst_rtype

// ROB general
#define ROB_HEAD                                                               \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_head
#define ROB_TAIL                                                               \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT__rob_tail
#define ROB_EMPTY                                                              \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob__DOT___empty_T_1


#define DEST_UNUSED 2

// Fetch target queue
#define FTQ_ENTRY(x)                                                           \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__ftq__DOT__pcs_##x
#define ROB_PC_LOB_MASK ((1ULL << 6) - 1)

// PC calculation (RoB bank #1)
#define GET_ROB_PC(top, i)                                                     \
  ((getFetchQueuePc(top, top->ROB_FTQ_IDX(i)) & ~ROB_PC_LOB_MASK) +            \
   (top->ROB_PC(i) & ROB_PC_LOB_MASK) - (top->ROB_EDGE_INST(i) * 2))

// Committing instruction information.
#define ROB_COMMITTING_PC                                                      \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_0_pc_lob
#define ROB_COMMITTING_FTQ_IDX                                                 \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_0_ftq_idx
#define ROB_COMMITTING_EDGE_INST                                               \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__rob_io_commit_uops_0_edge_inst

// Comitting Instr (RoB bank #1)
#define GET_COMMITTING_PC(top)                                                     \
  ((getFetchQueuePc(top->rootp, top->rootp->ROB_COMMITTING_FTQ_IDX) & ~ROB_PC_LOB_MASK) +            \
   (top->rootp->ROB_COMMITTING_PC & ROB_PC_LOB_MASK) - (top->rootp->ROB_COMMITTING_EDGE_INST * 2))

