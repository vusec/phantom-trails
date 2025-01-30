#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <iostream>

#include "top.h"

#ifdef SmallFuzzConfig
#define N_LDQ_ENTRIES   8
#define N_STQ_ENTRIES   8
#else
#define N_LDQ_ENTRIES   16
#define N_STQ_ENTRIES   16
#endif

#define LDQ_VALID(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_valid
#define LDQ_UOP_ROB_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_rob_idx
#define LDQ_UOP_LDQ_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_ldq_idx
#define LDQ_UOP_STQ_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_stq_idx
#define LDQ_UOP_PDST(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_pdst
#define LDQ_UOP_MEM_CMD(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_mem_cmd
#define LDQ_UOP_MEM_SIZE(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_mem_size
#define LDQ_UOP_MEM_SIGNED(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_mem_signed
#define LDQ_UOP_IS_FENCE(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_is_fence
#define LDQ_UOP_IS_AMO(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_is_amo
#define LDQ_UOP_USES_LDQ(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_uses_ldq
#define LDQ_UOP_USES_STQ(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_uses_stq
#define LDQ_UOP_DST_RTYPE(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_dst_rtype
#define LDQ_UOP_FP_VAL(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_uop_fp_val
#define LDQ_ADDR_VALID(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_addr_valid
#define LDQ_ADDR_IS_VIRTUAL(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_addr_is_virtual
#define LDQ_ADDR_IS_UNCACHEABLE(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_addr_is_uncacheable
#define LDQ_EXECUTED(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_executed
#define LDQ_SUCCEEDED(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_succeeded
#define LDQ_ORDER_FAIL(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_order_fail
#define LDQ_OBSERVED(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_observed
#define LDQ_YOUNGEST_STQ_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_youngest_stq_idx
#define LDQ_FORWARD_STD_VAL(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_forward_std_val
#define LDQ_FORWARD_STQ_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_forward_stq_idx
#define LDQ_ADDR(N)   \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_##N##_bits_addr_bits

#define STQ_VALID(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_valid
#define STQ_UOP_ROB_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_rob_idx
#define STQ_UOP_LDQ_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_ldq_idx
#define STQ_UOP_STQ_IDX(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_stq_idx
#define STQ_UOP_PDST(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_pdst
#define STQ_UOP_EXCEPTION(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_exception
#define STQ_UOP_MEM_CMD(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_mem_cmd
#define STQ_UOP_MEM_SIZE(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_mem_size
#define STQ_UOP_MEM_SIGNED(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_mem_signed
#define STQ_UOP_IS_FENCE(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_is_fence
#define STQ_UOP_IS_AMO(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_is_amo
#define STQ_UOP_USES_LDQ(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_uses_ldq
#define STQ_UOP_USES_STQ(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_uses_stq
#define STQ_UOP_DST_RTYPE(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_uop_dst_rtype
#define STQ_ADDR_VALID(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_addr_valid
#define STQ_ADDR_IS_VIRTUAL(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_addr_is_virtual
#define STQ_DATA_VALID(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_data_valid
#define STQ_COMMITTED(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_committed
#define STQ_SUCCEEDED(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_succeeded
#define STQ_ADDR(N)   \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_addr_bits
#define STQ_DATA(N) \
     TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_##N##_bits_data_bits

#define LDQ_HEAD \
        TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_head
#define LDQ_TAIL \
        TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__ldq_tail
#define STQ_HEAD \
        TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_head
#define STQ_TAIL \
        TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__stq_tail


#define LSU_LOAD_TRANSLATED \
        TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__lsu__DOT__do_ld_search_0

struct LDQEntry {
    uint8_t idx = 0;
    uint8_t valid = 0;
    uint64_t addr = 0;
    uint8_t uop_rob_idx = 0;
    uint8_t uop_ldq_idx = 0;
    uint8_t uop_stq_idx = 0;
    uint8_t uop_pdst = 0;
    uint8_t uop_mem_cmd = 0;
    uint8_t uop_mem_size = 0;
    uint8_t uop_mem_signed = 0;
    uint8_t uop_is_fence = 0;
    uint8_t uop_is_amo = 0;
    uint8_t uop_uses_ldq = 0;
    uint8_t uop_uses_stq = 0;
    uint8_t uop_dst_rtype = 0;
    uint8_t uop_fp_val = 0;
    uint8_t addr_valid = 0;
    uint8_t addr_is_virtual = 0;   // Virtual address, we got a TLB miss
    uint8_t addr_is_uncacheable = 0; // Uncacheable, wait until head of ROB to execute
    uint8_t executed = 0;  // load sent to memory, reset by NACKs
    uint8_t succeeded = 0;
    uint8_t order_fail = 0;
    uint8_t observed = 0;
    uint8_t youngest_stq_idx = 0; // index of the oldest store younger than us
    uint8_t forward_std_val = 0;
    uint8_t forward_stq_idx = 0; // Which store did we get the store-load forward from?

    std::string to_string(std::string prefix="") const {
        std::ostringstream out;
        out << prefix << "{ LDQ ENTRY: " << (uint64_t)idx << ", "
            << prefix << "valid: " << std::hex <<(uint64_t)valid << ", "
            << prefix << "addr: 0x" << std::hex <<(uint64_t)addr << ", "
            << prefix << "uop_rob_idx: " << std::hex <<(uint64_t)uop_rob_idx << ", "
            << prefix << "uop_ldq_idx: " << std::hex <<(uint64_t)uop_ldq_idx << ", "
            << prefix << "uop_stq_idx: " << std::hex <<(uint64_t)uop_stq_idx << ", "
            << prefix << "uop_pdst: "    << std::hex <<(uint64_t)uop_pdst << ", "
            << prefix << "uop_mem_cmd: " << std::hex <<(uint64_t)uop_mem_cmd << ", "
            << prefix << "uop_mem_size: "   << std::hex <<(uint64_t)uop_mem_size << ", "
            << prefix << "uop_mem_signed: " << std::hex <<(uint64_t)uop_mem_signed << ", "
            << prefix << "uop_is_fence: "   << std::hex <<(uint64_t)uop_is_fence << ", "
            << prefix << "uop_is_amo: "     << std::hex <<(uint64_t)uop_is_amo << ", "
            << prefix << "uop_uses_ldq: "   << std::hex <<(uint64_t)uop_uses_ldq << ", "
            << prefix << "uop_uses_stq: "   << std::hex <<(uint64_t)uop_uses_stq << ", "
            << prefix << "uop_dst_rtype: "  << std::hex <<(uint64_t)uop_dst_rtype << ", "
            << prefix << "uop_fp_val: " << std::hex <<(uint64_t)uop_fp_val << ", "
            << prefix << "addr_valid: " << std::hex <<(uint64_t)addr_valid << ", "
            << prefix << "addr_is_virtual: "    << std::hex <<(uint64_t)addr_is_virtual << ", "
            << prefix << "addr_is_uncacheable: " << std::hex <<(uint64_t)addr_is_uncacheable << ", "
            << prefix << "executed: "   << std::hex <<(uint64_t)executed << ", "
            << prefix << "succeeded: "  << std::hex <<(uint64_t)succeeded << ", "
            << prefix << "order_fail: " << std::hex <<(uint64_t)order_fail << ", "
            << prefix << "observed: "   << std::hex <<(uint64_t)observed << ", "
            << prefix << "youngest_stq_idx: "   << std::hex <<(uint64_t)youngest_stq_idx << ", "
            << prefix << "forward_std_val: "    << std::hex <<(uint64_t)forward_std_val << ", "
            << prefix << "forward_stq_idx: "    << std::hex <<(uint64_t)forward_stq_idx
            << prefix << "}";
        return out.str();
    }
};

struct STQEntry {
    uint8_t idx = 0;
    uint64_t data = 0;
    uint8_t valid = 0;
    uint64_t addr = 0;
    uint8_t uop_rob_idx = 0;
    uint8_t uop_ldq_idx = 0;
    uint8_t uop_stq_idx = 0;
    uint8_t uop_pdst = 0;
    uint8_t uop_exception = 0;
    uint8_t uop_mem_cmd = 0;
    uint8_t uop_mem_size = 0;
    uint8_t uop_mem_signed = 0;
    uint8_t uop_is_fence = 0;
    uint8_t uop_is_amo = 0;
    uint8_t uop_uses_ldq = 0;
    uint8_t uop_uses_stq = 0;
    uint8_t uop_dst_rtype = 0;
    uint8_t addr_valid = 0;
    uint8_t addr_is_virtual = 0;
    uint8_t data_valid = 0;
    uint8_t committed = 0;
    uint8_t succeeded = 0;


    std::string to_string(std::string prefix="") const {
        std::ostringstream out;
        out << prefix << "{ STQ ENTRY: " << (uint64_t)idx << ", "
            << prefix << "data: 0x" << std::hex << (uint64_t)data << ", "
            << prefix << "valid: " << std::hex << (uint64_t)valid << ", "
            << prefix << "addr: 0x" << std::hex <<(uint64_t)addr << ", "
            << prefix << "uop_rob_idx: " << std::hex << (uint64_t)uop_rob_idx << ", "
            << prefix << "uop_ldq_idx: " << std::hex << (uint64_t)uop_ldq_idx << ", "
            << prefix << "uop_stq_idx: " << std::hex << (uint64_t)uop_stq_idx << ", "
            << prefix << "uop_pdst: " << std::hex << (uint64_t)uop_pdst << ", "
            << prefix << "uop_exception: " << std::hex << (uint64_t)uop_exception << ", "
            << prefix << "uop_mem_cmd: " << std::hex << (uint64_t)uop_mem_cmd << ", "
            << prefix << "uop_mem_size: " << std::hex << (uint64_t)uop_mem_size << ", "
            << prefix << "uop_mem_signed: " << std::hex << (uint64_t)uop_mem_signed << ", "
            << prefix << "uop_is_fence: " << std::hex << (uint64_t)uop_is_fence << ", "
            << prefix << "uop_is_amo: " << std::hex << (uint64_t)uop_is_amo << ", "
            << prefix << "uop_uses_ldq: " << std::hex << (uint64_t)uop_uses_ldq << ", "
            << prefix << "uop_uses_stq: " << std::hex << (uint64_t)uop_uses_stq << ", "
            << prefix << "uop_dst_rtype: " << std::hex << (uint64_t)uop_dst_rtype << ", "
            << prefix << "addr_valid: " << std::hex << (uint64_t)addr_valid << ", "
            << prefix << "addr_is_virtual: " << std::hex << (uint64_t)addr_is_virtual << ", "
            << prefix << "data_valid: " << std::hex << (uint64_t)data_valid << ", "
            << prefix << "committed: " << std::hex << (uint64_t)committed << ", "
            << prefix << "succeeded: " << std::hex << (uint64_t)succeeded
            << prefix << "}";
        return out.str();
    }
};

struct LSQState {
  LDQEntry ldq[N_LDQ_ENTRIES];
  STQEntry stq[N_STQ_ENTRIES];

  uint8_t ldq_head = 0;
  uint8_t ldq_tail = 0;
  uint8_t stq_head = 0;
  uint8_t stq_tail = 0;

  void readFrom(const TOP *top) {

    ldq_head = top->LDQ_HEAD;
    ldq_tail = top->LDQ_TAIL;
    stq_head = top->STQ_HEAD;
    stq_tail = top->STQ_TAIL;

    #define LDQ_ENTRY(x)                 \
        ldq[x] = {  .idx = x,            \
                    .valid  = top->LDQ_VALID(x),                \
                    .addr  = top->LDQ_ADDR(x),                \
                    .uop_rob_idx    = top->LDQ_UOP_ROB_IDX(x),  \
                    .uop_ldq_idx    = top->LDQ_UOP_LDQ_IDX(x),  \
                    .uop_stq_idx    = top->LDQ_UOP_STQ_IDX(x),  \
                    .uop_pdst   = top->LDQ_UOP_PDST(x),            \
                    .uop_mem_cmd    = top->LDQ_UOP_MEM_CMD(x),     \
                    .uop_mem_size   = top->LDQ_UOP_MEM_SIZE(x),    \
                    .uop_mem_signed = top->LDQ_UOP_MEM_SIGNED(x),  \
                    .uop_is_fence   = top->LDQ_UOP_IS_FENCE(x),    \
                    .uop_is_amo = top->LDQ_UOP_IS_AMO(x),         \
                    .uop_uses_ldq   = top->LDQ_UOP_USES_LDQ(x),   \
                    .uop_uses_stq   = top->LDQ_UOP_USES_STQ(x),   \
                    .uop_dst_rtype  = top->LDQ_UOP_DST_RTYPE(x),  \
                    .uop_fp_val = top->LDQ_UOP_FP_VAL(x),  \
                    .addr_valid = top->LDQ_ADDR_VALID(x),  \
                    .addr_is_virtual    = top->LDQ_ADDR_IS_VIRTUAL(x),  \
                    .addr_is_uncacheable    = top->LDQ_ADDR_IS_UNCACHEABLE(x),  \
                    .executed   = top->LDQ_EXECUTED(x),    \
                    .succeeded  = top->LDQ_SUCCEEDED(x),   \
                    .order_fail = top->LDQ_ORDER_FAIL(x),  \
                    .observed   = top->LDQ_OBSERVED(x),    \
                    .youngest_stq_idx   = top->LDQ_YOUNGEST_STQ_IDX(x), \
                    .forward_std_val    = top->LDQ_FORWARD_STD_VAL(x),  \
                    .forward_stq_idx    = top->LDQ_FORWARD_STQ_IDX(x)   \
                 }

        LDQ_ENTRY(0);
        LDQ_ENTRY(1);
        LDQ_ENTRY(2);
        LDQ_ENTRY(3);
        LDQ_ENTRY(4);
        LDQ_ENTRY(5);
        LDQ_ENTRY(6);
        LDQ_ENTRY(7);
#ifndef SmallFuzzConfig
        LDQ_ENTRY(8);
        LDQ_ENTRY(9);
        LDQ_ENTRY(10);
        LDQ_ENTRY(11);
        LDQ_ENTRY(12);
        LDQ_ENTRY(13);
        LDQ_ENTRY(14);
        LDQ_ENTRY(15);
#endif
    #undef LDQ_ENTRY

    #define STQ_ENTRY(x)                                            \
        stq[x] = {  .idx = x,                                       \
                    .data       = top->STQ_DATA(x),                 \
                    .valid       = top->STQ_VALID(x),               \
                    .addr       = top->STQ_ADDR(x),                 \
                    .uop_rob_idx = top->STQ_UOP_ROB_IDX(x),         \
                    .uop_ldq_idx = top->STQ_UOP_LDQ_IDX(x),         \
                    .uop_stq_idx = top->STQ_UOP_STQ_IDX(x),         \
                    .uop_pdst    = top->STQ_UOP_PDST(x),            \
                    .uop_exception  = top->STQ_UOP_EXCEPTION(x),    \
                    .uop_mem_cmd    = top->STQ_UOP_MEM_CMD(x),      \
                    .uop_mem_size   = top->STQ_UOP_MEM_SIZE(x),     \
                    .uop_mem_signed = top->STQ_UOP_MEM_SIGNED(x),   \
                    .uop_is_fence   = top->STQ_UOP_IS_FENCE(x),     \
                    .uop_is_amo   = top->STQ_UOP_IS_AMO(x),         \
                    .uop_uses_ldq = top->STQ_UOP_USES_LDQ(x),       \
                    .uop_uses_stq = top->STQ_UOP_USES_STQ(x),       \
                    .uop_dst_rtype = top->STQ_UOP_DST_RTYPE(x),     \
                    .addr_valid    = top->STQ_ADDR_VALID(x),        \
                    .addr_is_virtual = top->STQ_ADDR_IS_VIRTUAL(x), \
                    .data_valid      = top->STQ_DATA_VALID(x),      \
                    .committed       = top->STQ_COMMITTED(x),       \
                    .succeeded       = top->STQ_SUCCEEDED(x)        \
                 }

        STQ_ENTRY(0);
        STQ_ENTRY(1);
        STQ_ENTRY(2);
        STQ_ENTRY(3);
        STQ_ENTRY(4);
        STQ_ENTRY(5);
        STQ_ENTRY(6);
        STQ_ENTRY(7);
#ifndef SmallFuzzConfig
        STQ_ENTRY(8);
        STQ_ENTRY(9);
        STQ_ENTRY(10);
        STQ_ENTRY(11);
        STQ_ENTRY(12);
        STQ_ENTRY(13);
        STQ_ENTRY(14);
        STQ_ENTRY(15);
#endif
    #undef STQ_ENTRY

  }

  std::string to_string(std::string prefix="") const {
    std::ostringstream out;

    out << "ldq_head:   " << std::hex << (uint64_t)ldq_head << "\n";
    out << "ldq_tail:   " << std::hex << (uint64_t)ldq_tail << "\n";
    out << "stq_head:   " << std::hex << (uint64_t)stq_head << "\n";
    out << "stq_tail:   " << std::hex << (uint64_t)stq_tail << "\n";

    out << "LoadQueue: [\n";
    for (int i = ldq_head; i != ldq_tail; i = (i + 1) % N_LDQ_ENTRIES)
        if (ldq[i].valid)
            out << ldq[i].to_string("    ") << ",\n";
    out << "]\n";

    out << "StoreQueue: [\n";
    for (int i = stq_head; i != stq_tail; i = (i + 1) % N_STQ_ENTRIES)
        if (stq[i].valid)
            out << stq[i].to_string("    ") << ",\n";
    out << "]\n";

    return out.str();
  }
};
