#pragma once

#define ICACHE_WAY_SIZE (512 * sizeof(uint64_t))
#define ICACHE_WAY(i)                                                                                                                                                                                         \
  TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__frontend__DOT__icache__DOT__dataArrayWay_##i##__DOT__dataArrayWay_0_ext__DOT__mem_0_0__DOT__ram \
      .m_storage
