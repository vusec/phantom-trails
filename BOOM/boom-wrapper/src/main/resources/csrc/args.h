#pragma once

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

// For TaintSource.
#include <Taint.h>

/// Used to split lines of the code regions file.
static std::vector<std::string> split(const std::string &s, const char delim) {
  std::vector<std::string> result;
  std::stringstream ss(s);
  std::string item;

  while (getline(ss, item, delim)) {
    result.push_back(item);
  }

  return result;
}

/// Command-line argument parsing.
struct args_t {
public:
  /// Path of the ROM image.
  const char *romPath = nullptr;
  // Code to run after ROM but before the program.
  const char *initCodePath = nullptr;
  /// Path of the program to run. Program must be in plain binary (i.e. no ELF).
  const char *programPath = nullptr;
  /// Used to determine how much to run the Spike simulation for.
  uint64_t spikeCyclesPerInst = 20;

public:
  /// Toggle MDS detector.
  bool mds = false;
  /// Cycle in which the internal buffers are valid and ready to be tainted.
  uint64_t mdsTaintCycle = 0;
  /// User-mode initialization code, appended to initCode.
  const char *mdsInitPath = nullptr;

public:
  /// Specify more than one code region, mainly addded to handle SpecDoctor.
  std::vector<std::pair<uint64_t, uint64_t>> codeRegions;

public:
  bool printCycles = false;
  bool printROB = false;
  bool printLSQ = false;
  bool printRegFile = false;
  bool printBIM = false;
  bool printLoop = false;
  bool printMSHR = false;
  bool verboseSim = false;
  bool verboseISA = false;
  bool printCrashReport = false;

  // Dump the whole content of the DRAM before start.
  std::string memdumpPath;

  // If the initialization is provided already in the target binary, this
  // indicates the length in bytes of such initialization code.
  // Used for debugging.
  uint64_t initSize;

public:
  /// Constructor.
  args_t(int argc, char **argv) {
    if (argc < 3) {
      std::cerr << "Usage: " << argv[0]
                << " <BIN_PATH> <ROM_PATH> [cyclesPerInst] [initCodePath] \n";
      exit(-1);
    }
    // ----------- Mandatory args.
    programPath = argv[1];
    romPath = argv[2];
    // ----------- Optional args.
    if (argc > 3)
      spikeCyclesPerInst = atoi(argv[3]);
    if (argc > 4)
      initCodePath = argv[4];
    // -----------  ENV args.
    // Initialization.
    if (std::getenv("INIT_SIZE"))
      initSize = std::stoul(std::getenv("INIT_SIZE"), nullptr, 16);
    // Logging.
    printCycles = std::getenv("PRINT_CYCLES");
    printROB = std::getenv("PRINT_ROB");
    printLSQ = std::getenv("PRINT_LSQ");
    printRegFile = std::getenv("PRINT_REGFILE");
    printBIM = std::getenv("PRINT_BIM");
    printLoop = std::getenv("PRINT_LOOP");
    printMSHR = std::getenv("PRINT_MSHR");
    verboseSim = std::getenv("VERBOSE_SIM");
    verboseISA = std::getenv("VERBOSE_ISA");
    printCrashReport = std::getenv("CRASH_REPORT");

    if (std::getenv("MEMDUMP")) {
      memdumpPath = std::string(programPath) + ".memdump";
    }
    // MDS.
    if (std::getenv("MDS")) {
      mds = true;
      if (std::getenv("MDS_TAINT_CYCLE")) {
        std::string s(std::getenv("MDS_TAINT_CYCLE"));
        mdsTaintCycle = std::stoul(s);
      }
      if (std::getenv("MDS_INIT")) {
        mdsInitPath = std::getenv("MDS_INIT");
      }
    }
    // Parse code regions (maily for SpecDoctor comparison).
    if (std::getenv("CODE_REGIONS")) {
      std::ifstream f;
      f.open(std::getenv("CODE_REGIONS"));

      for (std::string line; std::getline(f, line);) {
        // Each line in the code_region file should be in the form:
        // 0x{start},0xend
        std::vector<std::string> splitted = split(line, ',');
        codeRegions.push_back({std::stoul(splitted[0], nullptr, 16),
                               std::stoul(splitted[1], nullptr, 16)});
      }
    }
  }
};
