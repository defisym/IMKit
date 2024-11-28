#pragma once

#include "CPUAffinity/CPUAffinity.h"

namespace Intel13700K {
    inline auto defaultInfo = CPUInfo{ .pCore = 8, .eCore = 8, .bHyperThread = true };
    inline auto customInfo = CPUInfo{ .pCore = 8, .eCore = 8, .bHyperThread = false };
}
