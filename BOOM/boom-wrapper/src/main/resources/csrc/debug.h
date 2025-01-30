#pragma once

#include <stdio.h>

void enableDebugPrints();
void disableDebugPrints();
bool debugPrintsEnabled();

#ifdef DEBUG
  #define TRACE(...) \
    if (debugPrintsEnabled()) fprintf(stderr, __VA_ARGS__);
#else
  #define TRACE(...) (void)0
#endif
