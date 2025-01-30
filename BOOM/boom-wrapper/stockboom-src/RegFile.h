#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <iostream>

#include "top.h"

#define IREGFILE_SIZE (80 * sizeof(uint64_t))
#define IREGFILE                                                                                                                                \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__iregfile__DOT__regfile \
      .m_storage

#define FPREGFILE_SIZE (60 * 3 * sizeof(uint64_t))
#define FPREGFILE                                                                                                                                                 \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__core__DOT__fp_pipeline__DOT__fregfile__DOT__regfile \
      .m_storage


namespace regfile {

  /// Print our the content of the whole register file.
  static std::string printRegFile(const TOP *top) {
      std::ostringstream out;

      uint8_t *regfile = (uint8_t *)(top->IREGFILE);

      for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
          out << "[0x" << std::hex << (uint32_t)(i*8 + j) << "]  0x";
          for (int k = 0; k < 8; k++) {
            out << std::setfill('0') << std::setw(2) << std::hex << (uint32_t)(regfile[((i*8+j)*8)+k]);
          }
          out << "\t";
        }
        out << "\n";
      }

      return out.str();
  }
}
