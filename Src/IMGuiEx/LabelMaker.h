#pragma once

#include <string>

// imgui will display content before ##, which can be the same
// but after part as label must be unique
std::string ConnectLabel(const char* displayName, const char* label);