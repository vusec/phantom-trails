#pragma once

#include <stdint.h>
#include <vector>
#include <queue>


///
/// Memory used during simulation.
///
/// This class implements both the functions that are called from the
/// cycle-accurate verilog simulation (which simulates the bus activity)
/// and the C++ simulation driver (which populates and taints the memory).
struct SimMem {
  uint64_t memSize;
  uint64_t wordSize;
  uint64_t lineSize;
  uint64_t baseAddr;

  uint8_t *data;

  SimMem(uint64_t baseAddr, uint64_t memSize, uint64_t wordSize,
         uint64_t lineSize);
  ~SimMem() { /* Do nothing: faster. */ }

// --------------- Methods used by the simulation driver -----------------
  /// Align address to word size.
  uint64_t align(uint64_t addr) { return (addr / wordSize) * wordSize; }

  /// Write the content of src into dst, using strb as strobe.
  /// NB: Write data strobes: The write data strobe signal is used by a manager
  /// to tell a subordinate which bytes of the data bus are required.
  /// https://developer.arm.com/documentation/102202/0300/Channel-signals
  void write(uint64_t dst, uint8_t *src, uint64_t size, uint64_t strb = 0xFF);

  /// Read n bytes from addr.
  std::vector<uint8_t> read(uint64_t addr, uint64_t n);

  /// Read wordSize bytes from addr.
  std::vector<uint8_t> read(uint64_t addr) { return read(addr, wordSize); }

  /// Dump memory content to a file.
  void dumpMemory(unsigned long len, const char *fname) const;

// --------------- Signals connected to the verilated model -----------------
  bool writeAddrReady() { return !curStore.inFlight; }
  bool writeDataReady() { return curStore.inFlight; }
  bool writeResponseValid() { return !writeRespQueue.empty(); }
  uint64_t writeResponse() { return 0; }
  uint64_t writeResponseId() {
    return writeResponseValid() ? writeRespQueue.front() : 0;
  }

  bool readAddrReady() { return true; }
  bool readDataValid() { return !readQueue.empty(); }
  uint64_t readDataResp() { return 0; }
  uint64_t readDataId() { return readDataValid() ? readQueue.front().id : 0; }
  void *readData() {
    return readDataValid() ? &readQueue.front().data[0] : &dummy[0];
  }
  bool isLastRead() { return readDataValid() ? readQueue.front().last : false; }

  /// Clock tick function, called from the verilated model.
  /// AR = address read
  /// AW = address write
  /// R = read
  /// W = write
  /// B = write response
  void tick(bool reset, bool arValid, uint64_t arAddr, uint64_t arId,
            uint64_t arSize, uint64_t arLen, bool awValid, uint64_t awAddr,
            uint64_t awId, uint64_t awSize, uint64_t awLen, bool wValid,
            uint64_t wStrb, void *wData, bool wLast, bool rReady, bool bReady);

// --------------- Members for verilated model support -----------------
  struct InFlightStore {
    bool inFlight = false;
    uint64_t addr = 0;
    uint64_t id = 0;
    uint64_t size = 0;
    uint64_t count = 0;
  } curStore;
  uint64_t cycle;

  struct MemReadResponse {
    uint64_t id = 0;
    std::vector<uint8_t> data = {};
    bool last = false;
  };
  std::queue<MemReadResponse> readQueue;
  std::queue<uint64_t> writeRespQueue;
  std::vector<uint8_t> dummy;
};
