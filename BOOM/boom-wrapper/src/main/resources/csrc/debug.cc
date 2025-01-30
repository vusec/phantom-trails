#include "debug.h"

static bool debug = false;

void enableDebugPrints() { debug = true; }
void disableDebugPrints() { debug = false; }
bool debugPrintsEnabled() { return debug; }
