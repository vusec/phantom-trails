
#include <stdint.h>
#include <svdpi.h>
#include <vpi_user.h>

#include "SimMem.h"
#include "FakeDRAM.h"


static SimMem *memory = nullptr;

void FakeDRAM::setSimulationDRAM(SimMem* mem) {
  memory = mem;
}

/// DPI-C function, called directly from the verilog model (FakeDRAM.v).
extern "C" void memory_tick(unsigned char reset,

                            unsigned char ar_valid, unsigned char *ar_ready,
                            int ar_addr, int ar_id, int ar_size, int ar_len,

                            unsigned char aw_valid, unsigned char *aw_ready,
                            int aw_addr, int aw_id, int aw_size, int aw_len,

                            unsigned char w_valid, unsigned char *w_ready,
                            int w_strb, long long w_data, unsigned char w_last,

                            unsigned char *r_valid, unsigned char r_ready,
                            int *r_id, int *r_resp, long long *r_data,
                            unsigned char *r_last,

                            unsigned char *b_valid, unsigned char b_ready,
                            int *b_id, int *b_resp) {
  if (memory == nullptr)
    return;

  memory->tick(reset,
                   ar_valid, ar_addr, ar_id, ar_size, ar_len,
                   aw_valid, aw_addr, aw_id, aw_size, aw_len,
                   w_valid, w_strb, &w_data, w_last,
                   r_ready, b_ready);

  *ar_ready = memory->readAddrReady();
  *aw_ready = memory->writeAddrReady();
  *w_ready = memory->writeDataReady();
  *r_valid = memory->readDataValid();
  *r_id = memory->readDataId();
  *r_resp = memory->readDataResp();
  *r_data = *((long *)memory->readData());
  *r_last = memory->isLastRead();
  *b_valid = memory->writeResponseValid();
  *b_id = memory->writeResponseId();
  *b_resp = memory->writeResponse();
}
