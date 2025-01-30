#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <vector>

#include "abstract_device.h"
#include "memtracer.h"

class FakeDevice : public abstract_device_t {
public:
  FakeDevice(uint64_t base, uint64_t size, const char *name)
      : base(base), size(size), name(name) {
    mem = (uint8_t *)malloc(size);
  }
  FakeDevice(const FakeDevice &that) = delete;
  ~FakeDevice() {
    // don't free anything
  }

  bool load(reg_t addr, size_t len, uint8_t *bytes) override {
    if (addr >= size)
      return false;

    uint8_t *realAddr = &mem[addr];
    memcpy(bytes, realAddr, len);

    return true;
  }

  bool store(reg_t addr, size_t len, const uint8_t *bytes) override {
    if (addr >= size)
      return false;

    uint8_t *realAddr = &mem[addr];
    memcpy(realAddr, bytes, len);

    return true;
  }

private:
  uint64_t size;
  uint64_t base;
  uint8_t *mem;
  const char *name;
};

class FileMem : public abstract_device_t {
public:
  FileMem(uint64_t base, uint64_t size) : memBase(base), memSize(size) {
    mem = (uint8_t *)malloc(size);
    if (mem == NULL) {
      printf("[SimMem] mmap for backing storage failed");
      exit(-1);
    }
  }
  FileMem(const FileMem &that) = delete;
  ~FileMem() {
    // Don't free, faster.
  }

  bool load(reg_t addr, size_t len, uint8_t *bytes) override {
    if (addr >= memSize)
      return false;

    // std::cerr << "[MEM] Loading from " << std::hex << addr << "\n";

    if (shouldRecordAddresses)
      loads.push_back({addr + memBase, len});

    uint8_t *realAddr = &mem[addr];
    memcpy(bytes, realAddr, len);

    return true;
  }

  bool store(reg_t addr, size_t len, const uint8_t *bytes) override {
    if (addr >= memSize)
      return false;

    // std::cerr << "[MEM] Storing from " << std::hex << addr << "\n";

    if (shouldRecordAddresses)
      stores.push_back({addr + memBase, len});

    uint8_t *realAddr = &mem[addr];
    memcpy(realAddr, bytes, len);

    return true;
  }

  uint64_t loadProgram(const char *fname) {
    int fd = open(fname, O_RDONLY);
    if (fd < 0)
      return 0;

    struct stat s;
    const int status = fstat(fd, &s);
    uint64_t size = s.st_size;

    char *f =
        reinterpret_cast<char *>(mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0));

    memcpy(mem, f, std::min(size, memSize));

    return size;
  }

  uint64_t loadProgram(uint8_t *buf, uint64_t len) {
    if (buf == nullptr)
      return 0;
    memcpy(mem, buf, std::min(len, memSize));

    return len;
  }

  void traceAddresses(bool val) { shouldRecordAddresses = val; }

public:
  std::vector<std::pair<uint64_t, size_t>> loads;
  std::vector<std::pair<uint64_t, size_t>> stores;

public:
  uint64_t memSize;
  uint64_t memBase;
  uint8_t *mem;
  bool shouldRecordAddresses;

public:
  void reset() {
    loads.clear();
    stores.clear();
  }
};

/*
 * Record the addresses of all fetched instructions.
 */
struct FetchRecorder : public memtracer_t {

public:
  uint64_t last_accessed = 0;
  uint64_t n_same_inst = 0;
  bool stuck = false;

public:
  bool interested_in_range(uint64_t begin, uint64_t end,
                           access_type type) override {
    return type == FETCH;
  }

  void trace(uint64_t addr, size_t bytes, access_type type) override {
    // std::cerr << "[REC] Fetching from " << std::hex << addr << "\n";

    if (type == FETCH) {
      for (int i = 0; i < bytes; i++)
        fetches.insert(addr + i);

      if (addr == last_accessed) {
        n_same_inst++;
      } else {
        n_same_inst = 0;
        last_accessed = addr;
      }

      if (n_same_inst > 10)
        stuck = true;
    }
  }

  void clean_invalidate(uint64_t addr, size_t bytes, bool clean,
                        bool inval) override {}

public:
  std::set<uint64_t> fetches;

public:
  void reset() { fetches.clear(); }
};

/*
 * Record the addresses of all loads & stores.
 */
struct MemRecorder : public memtracer_t {
public:
  bool interested_in_range(uint64_t begin, uint64_t end,
                           access_type type) override {
    return type == LOAD || type == STORE;
  }

  void trace(uint64_t addr, size_t bytes, access_type type) override {
    if (type == STORE)
      for (int i = 0; i < bytes; i++)
        stores.insert(addr + i);

    if (type == LOAD) {
      for (int i = 0; i < bytes; i++) {
        // Optimization for store-to-load forwarding.
        if (stores.count(addr + i) == 0) {
          loads.insert(addr + i);
          // std::cerr << "Inserted " << std::hex << addr + i << "\n";
        } else {
          // std::cerr << "Skipped " << std::hex << addr + i << "\n";
        }
      }
    }
  }

  void clean_invalidate(uint64_t addr, size_t bytes, bool clean,
                        bool inval) override {}

public:
  std::set<uint64_t> loads;
  std::set<uint64_t> stores;

public:
  void reset() {
    loads.clear();
    stores.clear();
  }
};
