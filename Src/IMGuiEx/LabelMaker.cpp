#include "LabelMaker.h"

#include <format>

std::string ConnectLabel(const char* displayName, const char* label) {
    return std::format("{}##{}", displayName, label);
}
