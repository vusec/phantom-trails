#include "stdint.h"
#include <vector>

extern "C" {
struct SimResult {
  // Addresses accessed by the program. Elements are ordered and unique.
  uint64_t *addrs;
  uint64_t num_addrs;
  // Address of executed instructions. Elements are ordered and unique.
  uint64_t *insts;
  uint64_t num_insts;
  // Number of instructions that were executed during simulation.
  uint64_t num_retired;
};

/// Initialize the Spike architectural simulator.
/// @param ramBase Base address of the RAM.
/// @param ramSize Size of the RAM in bytes.
/// @param romPath Path of the ROM image.
/// @param verbose Print every instruction executed by the simulation.
void initSpikeSim(uint64_t ramBase, uint64_t ramSize, const char *romPath,
                  /*bool*/ int verbose);

void resetSpikeSim();

/// [MDS ONLY] Set a region of memory that we need to make sure is never
/// accessed by the program. This is used only to model MDS.
/// @param start virtual address of the beginning of the forbidden memory
/// @param end   virtual address of the end of the forbidden memory
void setSpikeForbiddenRegion(uint64_t start, uint64_t end);

/// Run a program on the architectural simulator.
/// @param program      Buffer containing the program to run.
/// @param programLen   Length of the `program` buffer.
/// @param maxCycles    Max number of cycles to run the simulation for.
///                     The actual number of executed instructions can be less
///                     than this, if the simulation meets an early-exit
///                     condition.
/// @param codeRegions  List of <start, end> address pairs that indicate where
///                     valid code is expected. By default, if this is empty,
///                     the whole program (0 to programLen) is considered a code
///                     region.
/// @param initSize     Size of the initialization code.
SimResult runSpikeSim(uint8_t *program, uint64_t programLen, uint64_t maxCycles,
                      std::vector<std::pair<uint64_t, uint64_t>> codeRegions, uint64_t initSize = 0);
}
