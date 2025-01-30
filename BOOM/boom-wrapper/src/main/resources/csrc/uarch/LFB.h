#pragma once

#include "top.h"

#include <sstream>

#define MSHR_SDQ_VALID \
    TestHarness__DOT__chiptop__DOT__system__DOT__tile_prci_domain__DOT__tile_reset_domain__DOT__boom_tile__DOT__dcache__DOT__mshrs__DOT__sdq_val

#define N_SDQ_ENTRIES 17

struct SdqEntry {
    bool valid;
};

struct MSHRs {
    SdqEntry sdq[N_SDQ_ENTRIES];

    void readFrom(const TOP* top) {
        uint32_t valids = top->MSHR_SDQ_VALID;

        for (int i = 0; i < N_SDQ_ENTRIES; i++) {
            sdq[i].valid = (bool)(valids & 1);
            valids = valids >> 1;
        }
    }

    std::string to_string() {
        std::ostringstream out;
        for (int i = 0; i < N_SDQ_ENTRIES; i++) {
            if (sdq[i].valid) {
                out << "ENTRY [" << i << "]  valid\n";
            }
        }
        return out.str();
    }
};
