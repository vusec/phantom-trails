#pragma once

#ifdef DFSAN
#include <sanitizer/dfsan_interface.h>
#define taint_label dfsan_label

static inline taint_label taint_read_label(uint8_t *rawByte, uint64_t size) {
  return dfsan_read_label(rawByte, size);
}

static inline void taint_set_label(taint_label lbl, uint8_t *buf,
                                   uint64_t size) {
  dfsan_set_label(lbl, buf, size);
}

#elif defined MSAN
#include </clang/llvm/compiler-rt/include/tainting.h>
#define taint_label uint8_t

static inline taint_label taint_read_label(uint8_t *rawByte, uint64_t size) {
  return Tainting::check(rawByte, size);
}

static inline void taint_set_label(taint_label lbl, uint8_t *buf,
                                   uint64_t size) {
  if (lbl == 0)
    Tainting::untaintPtr(buf, size);
  else
    Tainting::taintPtr(buf, size);
}

#endif

// Taint sources.

enum TaintSource { ALL = 0, MEMORY, STORE_BUFFER, LOAD_QUEUE, LFB, UNKNOWN };

static std::string to_string(TaintSource s) {
  switch (s) {
  case TaintSource::ALL:
    return "ALL";
    break;
  case TaintSource::MEMORY:
    return "MEMORY";
    break;
  case TaintSource::STORE_BUFFER:
    return "STORE_BUFFER";
    break;
  case TaintSource::LOAD_QUEUE:
    return "LOAD_QUEUE";
    break;
  case TaintSource::LFB:
    return "LFB";
    break;
  default:
    return "UNKNOWN_SOURCE";
  }
}

/// Since we have only one color, we need two rounds to detect MDS: first
/// is reported as Meltdown, then on the second round we taint only the
/// buffers, to be sure that the taint is cause by MDS.
static TaintSource getNextTaintSource(const TaintSource curSrc) {
  switch (curSrc) {
  case TaintSource::ALL:
    return TaintSource::STORE_BUFFER;
    break;

  // TODO: Add other buffers, for now we have only MDS-DB.
  default:
    return TaintSource::UNKNOWN;
  }
}
