#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include "top.h"

// --------------------------------- BIM

#define BIM_BANK_SIZE 2048
#define BIM_BANK_0                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_4__DOT__data__DOT__data_ext__DOT__mem_0_0__DOT__ram
#define BIM_BANK_1                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_4__DOT__data__DOT__data_ext__DOT__mem_0_1__DOT__ram
#define BIM_BANK_2                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_4__DOT__data__DOT__data_ext__DOT__mem_0_2__DOT__ram
#define BIM_BANK_3                                                             \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_4__DOT__data__DOT__data_ext__DOT__mem_0_3__DOT__ram

/// Bi-Modal table predictor (PHT).
struct Bim {
  uint8_t bimBank0[BIM_BANK_SIZE] = {0};
  uint8_t bimBank1[BIM_BANK_SIZE] = {0};
  uint8_t bimBank2[BIM_BANK_SIZE] = {0};
  uint8_t bimBank3[BIM_BANK_SIZE] = {0};

  void readFrom(const TOP *top) {
    memcpy(bimBank0, (uint8_t *)top->BIM_BANK_0.m_storage, BIM_BANK_SIZE);
    memcpy(bimBank1, (uint8_t *)top->BIM_BANK_1.m_storage, BIM_BANK_SIZE);
    memcpy(bimBank2, (uint8_t *)top->BIM_BANK_2.m_storage, BIM_BANK_SIZE);
    memcpy(bimBank3, (uint8_t *)top->BIM_BANK_3.m_storage, BIM_BANK_SIZE);
  }

  std::string printDiff(const TOP *top) {
    std::ostringstream out;

    for (int i = 0; i < BIM_BANK_SIZE; i++) {
      if (bimBank0[i] != top->BIM_BANK_0.m_storage[i])
        out << "[BIM] Bank: 0 Entry: " << std::hex << i
                  << "   old: " << (int)bimBank0[i]
                  << "  new: " << (int)top->BIM_BANK_0.m_storage[i] << "\n";

      if (bimBank1[i] != top->BIM_BANK_1.m_storage[i])
        out << "[BIM] Bank: 1 Entry: " << std::hex << i
                  << "   old: " << (int)bimBank1[i]
                  << "  new: " << (int)top->BIM_BANK_1.m_storage[i] << "\n";

      if (bimBank2[i] != top->BIM_BANK_2.m_storage[i])
        out << "[BIM] Bank: 2 Entry: " << std::hex << i
                  << "   old: " << (int)bimBank2[i]
                  << "  new: " << (int)top->BIM_BANK_2.m_storage[i] << "\n";

      if (bimBank3[i] != top->BIM_BANK_3.m_storage[i])
        out << "[BIM] Bank: 3 Entry: " << std::hex << i
                  << "   old: " << (int)bimBank3[i]
                  << "  new: " << (int)top->BIM_BANK_3.m_storage[i] << "\n";
    }

    readFrom(top);

    return out.str();
  }
};


// --------------------------------- LOOP

#define N_LOOP_ENTRIES 16
#define N_LOOP_COLS 4

#define LOOP_TAG(i, j) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##__DOT__entries_##j##_tag
#define LOOP_CONF(i, j) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##__DOT__entries_##j##_conf
#define LOOP_AGE(i, j) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##__DOT__entries_##j##_age
#define LOOP_P_CNT(i, j) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##__DOT__entries_##j##_p_cnt
#define LOOP_S_CNT(i, j) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##__DOT__entries_##j##_s_cnt

#define LOOP_F3_PRED_IN(i) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##_io_f3_pred_in;
#define LOOP_F3_PRED(i) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##_io_f3_pred;
#define LOOP_UPDATE_MISPRED(i) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##_io_update_mispredict;
#define LOOP_UPDATE_REPAIR(i) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##_io_update_repair;
#define LOOP_F3_REQ_FIRE(i) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##_io_f3_req_fire_REG;
#define LOOP_F3_META_S_CNT(i) \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__bpd__DOT__banked_predictors_0__DOT__components_0__DOT__columns_##i##_io_f3_meta_s_cnt;

struct LoopEntry {
  uint8_t col;
  uint8_t n_entry;

  uint16_t tag;
  uint8_t conf;
  uint8_t age;
  uint16_t p_cnt;
  uint16_t s_cnt;

  std::string to_string() const {
    std::ostringstream out;

    out << "     ENTRY[0x" << std:: hex  << (int)(col) << "][0x" << std:: hex  << (int)(n_entry) << "]  {";
    out << "tag: 0x" << std:: hex << (int)(tag) << ",  ";
    out << "conf: 0x" << std:: hex << (int)(conf) << ",  ";
    out << "age: 0x" << std:: hex << (int)(age) << ",  ";
    out << "p_cnt: 0x" << std:: hex << (int)(p_cnt) << ",  ";
    out << "s_cnt: 0x" << std:: hex << (int)(s_cnt);
    out << " }";

    return out.str();
  }

  std::string diff(const LoopEntry& other) const {
    std::ostringstream out;

    if(tag != other.tag)
      out << "tag: 0x" << std:: hex << (int)(tag) << ",  ";
    if(conf != other.conf)
      out << "conf: 0x" << std:: hex << (int)(conf) << ",  ";
    if(age != other.age)
      out << "age: 0x" << std:: hex << (int)(age) << ",  ";
    if(p_cnt != other.p_cnt)
      out << "p_cnt: 0x" << std:: hex << (int)(p_cnt) << ",  ";
    if(s_cnt != other.s_cnt)
      out << "s_cnt: 0x" << std:: hex << (int)(s_cnt);

    return out.str();
  }
};

struct LoopColumn {
  uint8_t n_col;

  LoopEntry entries[N_LOOP_ENTRIES];

  uint8_t f3_pred_in;
  uint8_t f3_pred;
  uint8_t update_mispred;
  uint8_t update_repair;
  uint8_t f3_req_fire_REG;
  uint16_t f3_meta_s_cnt;

  std::string to_string() const {
    std::ostringstream out;

    out << "COLUMN 0x" << std:: hex  << (int)(n_col) << " {\n";
    out << "  f3_pred_in: 0x" << std:: hex  << (int)(f3_pred_in) << "\n";
    out << "  f3_pred: 0x" << std:: hex  << (int)(f3_pred) << "\n";
    out << "  update_mispred: 0x" << std:: hex  << (int)(update_mispred) << "\n";
    out << "  update_repair: 0x" << std:: hex  << (int)(update_repair) << "\n";
    out << "  f3_req_fire_REG: 0x" << std:: hex  << (int)(f3_req_fire_REG) << "\n";
    out << "  f3_meta_s_cnt: 0x" << std:: hex  << (int)(f3_meta_s_cnt) << "\n";

    out << "  ENTRIES:\n  {\n";
    for (const LoopEntry& entry: entries) {
      out << entry.to_string() << "\n";
    }
    out << "  }\n";

    out << "}\n";

    return out.str();
  }

  std::string diff(const LoopColumn& other) const {
    std::ostringstream out;

    if(f3_pred_in != other.f3_pred_in)
      out << "  f3_pred_in: " << (int)(f3_pred_in) << "\n";
    if(f3_pred != other.f3_pred)
      out << "  f3_pred: " << (int)(f3_pred) << "\n";
    if(update_mispred != other.update_mispred)
      out << "  update_mispred: " << (int)(update_mispred) << "\n";
    if(update_repair != other.update_repair)
      out << "  update_repair: " << (int)(update_repair) << "\n";
    if(f3_req_fire_REG != other.f3_req_fire_REG)
      out << "  f3_req_fire_REG: " << (int)(f3_req_fire_REG) << "\n";
    if(f3_meta_s_cnt != other.f3_meta_s_cnt)
      out << "  f3_meta_s_cnt: " << (int)(f3_meta_s_cnt) << "\n";

    for (int i = 0; i < N_LOOP_ENTRIES; i++) {
      auto diff = entries[i].diff(other.entries[i]);
      if (diff != "")
        out << "     ENTRY[" << (int)(i) << "] {" << diff << "}\n";
    }

    return out.str();
  }
};

struct LoopPredictor {
  LoopColumn cols[N_LOOP_COLS];


  void readFrom(const TOP *top) {
    #define READ_LOOP_COL(i)                                    \
      cols[i].n_col = i;                                        \
      cols[i].f3_pred_in = top->LOOP_F3_PRED_IN(i);             \
      cols[i].f3_pred = top->LOOP_F3_PRED(i);                   \
      cols[i].update_mispred = top->LOOP_UPDATE_MISPRED(i);     \
      cols[i].update_repair = top->LOOP_UPDATE_REPAIR(i);       \
      cols[i].f3_req_fire_REG = top->LOOP_F3_REQ_FIRE(i);       \
      cols[i].f3_meta_s_cnt = top->LOOP_F3_META_S_CNT(i);

    READ_LOOP_COL(0);
    READ_LOOP_COL(1);
    READ_LOOP_COL(2);
    READ_LOOP_COL(3);

    #undef READ_LOOP_COL

    #define READ_LOOP_ENTRY(i,j)              \
      cols[i].entries[j] = {                    \
          .col = i,                           \
          .n_entry = j,                       \
          .tag = top->LOOP_TAG(i,j),          \
          .conf = top->LOOP_CONF(i,j),        \
          .age = top->LOOP_AGE(i,j),          \
          .p_cnt = top->LOOP_P_CNT(i,j),      \
          .s_cnt = top->LOOP_S_CNT(i,j),      \
      };

    #define READ_WHOLE_COL(i)   \
    READ_LOOP_ENTRY(i, 0);      \
    READ_LOOP_ENTRY(i, 1);      \
    READ_LOOP_ENTRY(i, 2);      \
    READ_LOOP_ENTRY(i, 3);      \
    READ_LOOP_ENTRY(i, 4);      \
    READ_LOOP_ENTRY(i, 5);      \
    READ_LOOP_ENTRY(i, 6);      \
    READ_LOOP_ENTRY(i, 7);      \
    READ_LOOP_ENTRY(i, 8);      \
    READ_LOOP_ENTRY(i, 9);      \
    READ_LOOP_ENTRY(i, 10);      \
    READ_LOOP_ENTRY(i, 11);      \
    READ_LOOP_ENTRY(i, 12);      \
    READ_LOOP_ENTRY(i, 13);      \
    READ_LOOP_ENTRY(i, 14);      \
    READ_LOOP_ENTRY(i, 15);

    READ_WHOLE_COL(0);
    READ_WHOLE_COL(1);
    READ_WHOLE_COL(2);
    READ_WHOLE_COL(3);


    #undef READ_WHOLE_COL
    #undef READ_LOOP_ENTRY

  }

  std::string to_string() const {
    std::ostringstream out;

      for (const LoopColumn& col: cols) {
        out << col.to_string() << "\n";
      }

    return out.str();
  }

  std::string diff(const LoopPredictor& other, uint64_t cycle=0) const {
    std::ostringstream out;

    for (int i = 0; i < N_LOOP_COLS; i++) {
      auto diff = cols[i].diff(other.cols[i]);
      if (diff != "")
        out << "COLS[" << (int)(i) << "] {\n" << diff << "}\n";
    }

    return out.str();
  }

};
