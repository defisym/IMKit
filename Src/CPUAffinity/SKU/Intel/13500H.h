#pragma once

#include "CPUAffinity/CPUAffinity.h"

namespace Intel13500H {
    inline auto defaultInfo = CPUInfo{ .pCore = 4, .eCore = 8, .bHyperThread = true };
    inline auto customInfo = CPUInfo{ .pCore = 4, .eCore = 8, .bHyperThread = true };
}
