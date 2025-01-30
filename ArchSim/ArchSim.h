#pragma once
#include <sys/types.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "Mem.h"
#include "config.h"
#include "devices.h"
#include "log_file.h"
#include "mmu.h"
#include "platform.h"
#include "processor.h"
#include "riscv-isa-sim/riscv/cfg.h"
#include "riscv-isa-sim/riscv/platform.h"
#include "simif.h"

class mmu_t;

static inline std::string to_string(processor_t::IllegalJumpType jtype) {
  switch (jtype) {
  case processor_t::IllegalJumpType::NONE:
    return "NONE";
  case processor_t::IllegalJumpType::OUTSIDE_VALID_MEM:
    return "OUTSIDE_VALID_MEM";
  case processor_t::IllegalJumpType::NEWPC_TOO_NEAR:
    return "NEWPC_TOO_NEAR";
  case processor_t::IllegalJumpType::NEWPC_UNALIGNED:
    return "NEWPC_UNALIGNED";
  case processor_t::IllegalJumpType::MISALIGNED_LOAD:
    return "MISALIGNED_LOAD";
  default:
    return "Unknown";
  }
}

static bool paddr_ok(reg_t addr) { return (addr >> MAX_PADDR_BITS) == 0; }

/// Default configuration.
static const cfg_t build_config(uint64_t ram_base, uint64_t ram_size) {
  return {/*.initrd_bounds =*/{0, 0},
          /*.bootargs =*/nullptr,
          /*.isa =*/DEFAULT_ISA,
          /*.priv =*/DEFAULT_PRIV,
          /*.varch =*/DEFAULT_VARCH,
          /*.misaligned =*/false,
          /*.endianness =*/endianness_little,
          /*.pmpregions =*/16,
          /*.mem_layout =*/std::vector<mem_cfg_t>{{ram_base, ram_size}},
          /*.hartids =*/{0},
          /*.real_time_clint =*/false,
          /*.trigger_count =*/4};
}

/// Modded version of the Spike architectural emulator. Only one processor
/// is supported.
class ArchSim : public simif_t {
public:
  ArchSim(uint64_t ram_base, uint64_t ram_size, bool record_addresses,
          bool verbose, bool filters_enabled=true, bool record=true)
      : cfg(build_config(ram_base, ram_size)), isa(cfg.isa(), cfg.priv()),
        mem(ram_base, ram_size), reset_state(true),
        filters_enabled(filters_enabled), record_addresses(record) {

    // memory
    bus.add_device(ram_base, &mem);

    // interrupt controller
    clint.reset(
        new clint_t(this, CPU_HZ / INSNS_PER_RTC_TICK, cfg.real_time_clint()));
    bus.add_device(CLINT_BASE, clint.get());

    // other devices
    bus.add_device(PLIC_BASE, new FakeDevice(PLIC_BASE, PLIC_SIZE, "plic"));
    bus.add_device(0, new FakeDevice(0, 4096, "debug-controller@0"));
    bus.add_device(0x3000, new FakeDevice(0x3000, 4096, "error-device@3000"));
    bus.add_device(0x4000,
                   new FakeDevice(0x4000, 4096, "boot-address-reg@4000"));
    bus.add_device(0x100000,
                   new FakeDevice(0x100000, 4096, "clock-gater@100000"));
    bus.add_device(0x110000,
                   new FakeDevice(0x110000, 4096, "tile-reset-setter@110000"));
    bus.add_device(0x2010000,
                   new FakeDevice(0x2010000, 4096, "cache-controller@2010000"));

    allowed_devices.push_back({0x4000, 4096});
    allowed_devices.push_back({0x10000, 4096});
    allowed_devices.push_back({ram_base, ram_size});

    // processor
    proc = new processor_t(&isa, &cfg, this, 0, false, stderr, std::cerr,
                           ROM_ADDR);
    this->verbose = verbose;
    initProc(proc);
  }

  void initProc(processor_t* proc) {
    proc->set_pmp_num(cfg.pmpregions);
    proc->set_pmp_granularity(1 << PMP_SHIFT);
    proc->set_mmu_capability(IMPL_MMU_SV57);
    harts[0] = proc;

    // Register custom tracers
    if (record_addresses) {
      proc->get_mmu()->register_memtracer(&fetchRecorder);
      proc->get_mmu()->register_memtracer(&memRecorder);
      mem.traceAddresses(true);
    }

    // Debug prints
    if (verbose) {
      proc->enable_log_commits();
      proc->set_debug(true);
      std::cerr << "[ARCHSIM] Setup done.\n";
    }
  }

  ~ArchSim() {
    // Don't cleanup, faster.
  }

  void reset() {
    mem.reset();
    // TODO: do we really need to destroy?
    delete proc;
    proc = new processor_t(&isa, &cfg, this, 0, false, stderr, std::cerr,
                        ROM_ADDR);

    fetchRecorder.reset();
    memRecorder.reset();
    mmio_loads.clear();
    mmio_stores.clear();

    illegal_device = false;
  }

  // ------------------------- Program Loader ----------------------------------
  /// Load program from a buffer to the simulation RAM.
  void loadProgram(uint8_t *buf, uint64_t len, std::vector<std::pair<uint64_t, uint64_t>> code_regions, uint64_t init_size) {
    uint64_t size = mem.loadProgram(buf, len);

    proc->code_regions = code_regions;
    proc->init_end = mem.memBase + init_size;
    if (verbose)
      for (auto& reg : proc->code_regions)
        std::cerr << "[ARCHSIM] Code Region: (0x" << std::hex << reg.first
                  << ", 0x" << std::hex << reg.second << ")\n";
  }
  
  /// Load program from a file to the simulation RAM.
  void loadProgramFromFile(const char *file, uint64_t code_sz) {
    uint64_t size = mem.loadProgram(file);

    proc->code_regions.push_back(std::make_pair(mem.memBase, mem.memBase + std::min(code_sz, mem.memSize)));
  }

  /// Initialize content of the ROM from a buffer.
  void loadRom(const char *buf, uint64_t len) {
    std::vector<char> rom(buf, buf + len);

    const int align = 0x1000;
    rom.resize((rom.size() + align - 1) / align * align);

    boot_rom.reset(new rom_device_t(rom));
    bus.add_device(ROM_ADDR, boot_rom.get());
  }

  /// Initialize content of the ROM from a file.
  void loadRomFromFile(const char *path) {
    std::ifstream in(path);
    std::string contents((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());

    loadRom(contents.c_str(), contents.size());
  }

  // ------------------------- Execution ----------------------------------
  /// Execute n steps.
  void step(size_t n) { proc->step(n); }

  /// Run the simulation until max_cycles or an ealy-exit condition is found.
  /// Returns the number of cycles that the simulation was actually ran for.
  uint64_t run(uint64_t max_cycles) {
    uint64_t tick = 0;
    while (tick < max_cycles) {
      proc->step(1);
      tick++;

      // Early-exit.
      if (proc->illegal_inst or proc->unhandled_trap or
          proc->is_waiting_for_interrupt() or fetchRecorder.stuck) {
        if (verbose)
          std::cerr << "[ARCHSIM] Early exit: illegal_inst=" << proc->illegal_inst
                    << "  unhandled_trap=" << proc->unhandled_trap
                    << "  wfi=" << proc->is_waiting_for_interrupt()
                    << "  stuck= " << fetchRecorder.stuck << "\n";
        break;
      }

      // Filters.
      if (filters_enabled and
          (proc->illegal_jump or proc->self_modifying or illegal_device or proc->accessed_forbidden_region)) {
        if (verbose) {
          std::cerr << "[ARCHSIM] Discarded: illegal_jmp= " << proc->illegal_jump
                    << "   self_modifying= " << proc->self_modifying
                    << "   illegal_device= " << illegal_device
                    << "   accessed_forbidden_region= " << proc->accessed_forbidden_region << "\n";

          std::cerr << "   FaultPc = " << std::hex << proc->illegal_jump_pc
                    << "   Reason  = " << to_string(proc->illegal_jump_type)
                    << "\n";
        }

        if (proc->illegal_jump and proc->illegal_jump_type == processor_t::IllegalJumpType::OUTSIDE_VALID_MEM) {
        } else {
          return 0;
        }
      }
    }

    return tick;
  }

  // ------------------------- Memory Inspection ----------------------------------
  /// Get all memory addresses accessed by the simulation.
  std::set<uint64_t> getAccessedMem() {
    std::set<uint64_t> addresses;
    // DRAM addresses.
    addresses.merge(memRecorder.loads);
    // addresses.merge(memRecorder.stores);
    // MMIO addresses.
    for (const auto &[addr, size] : mmio_loads)
      for (uint8_t i = 0; i < size; ++i)
        addresses.insert(addr + i);
    for (const auto &[addr, size] : mmio_stores)
      for (uint8_t i = 0; i < size; ++i)
        addresses.insert(addr + i);
    return addresses;
  }

  /// Get all instruction addresses executed by the simulation.
  std::set<uint64_t> getExecutedInsts() {
    std::set<uint64_t> addresses;
    for (const auto& addr : fetchRecorder.fetches) {
      // Ignore boot code, which resides outside of the RAM address space.
      if (addr < mem.memBase or addr >= mem.memBase + mem.memSize)
        continue;

      addresses.insert(addr);
    }

    return addresses;
  }

  // -------------------------   Helpers   ----------------------------------
  /// Set a region of memory that should be never accessed by the program.
  void setForbiddenRegion(uint64_t start, uint64_t end) {
    proc->forbidden_regions.emplace_back(start, end);
  }

  /// Remember the addres of a specific symbol, for nicer debug prints.
  void register_symbol(uint64_t addr, const char *name) {
    symbols[addr] = name;
  }

  // Getters
  processor_t *getCore(size_t i) { return proc; }
  uint64_t getPC(size_t i) { return proc->get_state()->pc; }
  // Setters
  void enableFilters() { filters_enabled = true; }
  void disableFilters() { filters_enabled = false; }

private:
  const cfg_t cfg;
  isa_parser_t isa;
  FileMem mem;
  bus_t bus;
  processor_t *proc;
  std::unique_ptr<rom_device_t> boot_rom;
  std::unique_ptr<clint_t> clint;

  FetchRecorder fetchRecorder;
  MemRecorder memRecorder;
  std::vector<std::pair<uint64_t, size_t>> mmio_loads;
  std::vector<std::pair<uint64_t, size_t>> mmio_stores;

  bool verbose;
  bool filters_enabled;
  bool record_addresses;
  bool illegal_device;

  std::vector<std::pair<uint64_t, uint64_t>> allowed_devices;

  static const size_t INSNS_PER_RTC_TICK = 100; // 10 MHz clock for 1 BIPS core
  static const size_t CPU_HZ = 1000000000;      // 1GHz CPU
  static const uint64_t ROM_ADDR = 0x10000;
  uint64_t init_end = 0;

// --------------------- simif_t implementation  -----------------------------
private:
  /// Sym table used for nice prints
  const char *get_symbol(uint64_t addr) override { return symbols[addr]; }

  /// Converts simulation address to address in the fake dram
  char *addr_to_mem(reg_t addr) override {
    if (addr >= mem.memBase and addr < mem.memBase + mem.memSize)
      return (char *)&mem.mem[addr - mem.memBase];
    return NULL;
  }

  /// Check if we are reading from an allowed device.
  bool is_allowed_device(uint64_t addr, size_t len) {
    auto [reg, dev] = bus.find_device(addr);
    if (dev == nullptr)
      return true; // Illegal addresses are allowed.

    for (auto &d : allowed_devices)
      if (addr >= d.first and (addr + len) < (d.first + d.second))
        return true;

    return false;
  }
  /// Memory-Mapped IO simulation (load)
  bool mmio_load(reg_t addr, size_t len, uint8_t *bytes) override {
    illegal_device = not is_allowed_device(addr, len);

    if (addr + len < addr || !paddr_ok(addr + len - 1))
      return false;
    mmio_loads.push_back({addr, len});
    return bus.load(addr, len, bytes);
  }

  /// Memory-Mapped IO simulation (store)
  bool mmio_store(reg_t addr, size_t len, const uint8_t *bytes) override {
    illegal_device = not is_allowed_device(addr, len);

    if (addr + len < addr || !paddr_ok(addr + len - 1))
      return false;
    mmio_stores.push_back({addr, len});
    return bus.store(addr, len, bytes);
  }

  /// Callback to notifies the simulation that the processor has been reset
  void proc_reset(unsigned id) override { reset_state = true; }

  virtual const cfg_t &get_cfg() const override { return cfg; }

  virtual const std::map<size_t, processor_t *> &get_harts() const override {
    return harts;
  }

private:
  std::map<uint64_t, const char *> symbols;
  std::map<uint64_t, processor_t *> harts;
  bool reset_state;
};
