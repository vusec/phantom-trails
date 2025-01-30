#pragma once

#include "SimMem.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

#include "debug.h"

SimMem::SimMem(uint64_t baseAddr, uint64_t memSize, uint64_t wordSize, uint64_t lineSize)
    : baseAddr(baseAddr), memSize(memSize), wordSize(wordSize), lineSize(lineSize) {

  assert(wordSize > 0 && lineSize > 0 && (lineSize & (lineSize - 1)) == 0 &&
          lineSize % wordSize == 0);

  data = (uint8_t *)malloc(memSize);
  if (data == nullptr) {
    printf("[SimMem] mmap for backing storage failed");
    exit(-1);
  }

  dummy.resize(wordSize);
}


/// Write the content of src into dst, using strb as strobe.
/// NB: Write data strobes: The write data strobe signal is used by a manager
/// to tell a subordinate which bytes of the data bus are required.
/// https://developer.arm.com/documentation/102202/0300/Channel-signals
void SimMem::write(uint64_t dst, uint8_t *src, uint64_t size, uint64_t strb) {

  auto maxStrb = sizeof(uint64_t) * 8;
  assert(size <= maxStrb); // Ensure the strb is wide enough to
                            // support the desired transaction
  if (size != maxStrb)
    strb &= ((1 << size) - 1) << (dst % wordSize);

  uint8_t *base = data + align(dst % memSize);
  for (int i = 0; i < wordSize; i++) {
    if (strb & 1)
      base[i] = data[i];
    strb >>= 1;
  }
}

/// Read n bytes from addr.
std::vector<uint8_t> SimMem::read(uint64_t addr, uint64_t n) {
  uint8_t *base = data + (addr % memSize);
  return std::vector<uint8_t>(base, base + n);
}


/// Dump memory content to a file.
void SimMem::dumpMemory(unsigned long len, const char *fname) const {
  std::ofstream dumpFile(fname, std::ios::out | std::ios::binary);
  if (!dumpFile.is_open()) {
    TRACE("[ERROR] Couldn't open dump file %s.\n", fname);
    return;
  }

  for (int i = 0; i < len; i++)
    dumpFile << data[i];

  dumpFile.close();
}


/// Clock tick function, called from the verilated model.
/// AR = address read
/// AW = address write
/// R = read
/// W = write
/// B = write response
void SimMem::tick(bool reset, bool arValid, uint64_t arAddr, uint64_t arId,
          uint64_t arSize, uint64_t arLen, bool awValid, uint64_t awAddr,
          uint64_t awId, uint64_t awSize, uint64_t awLen, bool wValid,
          uint64_t wStrb, void *wData, bool wLast, bool rReady, bool bReady) {
  bool arFire = !reset && arValid && readAddrReady();
  bool awFire = !reset && awValid && writeAddrReady();
  bool wFire = !reset && wValid && writeDataReady();
  bool rFire = !reset && readDataValid() && rReady;
  bool bFire = !reset && writeResponseValid() && bReady;

  // Address Read
  if (arFire) {
    uint64_t start = align(arAddr);
    for (int i = 0; i <= arLen; i++) {
      readQueue.push(MemReadResponse{.id = arId,
                                      .data = read(start + i * wordSize),
                                      .last = i == arLen});
    }
  }

  // Address Write
  if (awFire) {
    curStore = {
        .inFlight = true,
        .addr = awAddr,
        .id = awId,
        .size = 1ULL << awSize,
        .count = awLen + 1,
    };
  }

  // Data Write
  if (wFire) {
    write(curStore.addr, (uint8_t *)wData, wStrb, curStore.size);
    curStore.addr += curStore.size;
    curStore.count--;

    if (curStore.count == 0) {
      curStore.inFlight = false;
      writeRespQueue.push(curStore.id);
      assert(wLast);
    }
  }

  // Write Response
  if (bFire)
    writeRespQueue.pop();
  // Read Response
  if (rFire)
    readQueue.pop();

  cycle++;

  // Reset
  if (reset) {
    while (!writeRespQueue.empty())
      writeRespQueue.pop();
    while (!readQueue.empty())
      readQueue.pop();
    cycle = 0;
  }
}
