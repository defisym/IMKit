#pragma once

#include <string>

// imgui will display content before ##, which can be the same
// but after part as label must be unique
struct LabelMaker {
    std::string MakeLabel(const char* displayName, const char* label);
};

