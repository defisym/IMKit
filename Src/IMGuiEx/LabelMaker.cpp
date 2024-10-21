#include "LabelMaker.h"

#include <format>

std::string LabelMaker::MakeLabel(const char* displayName, const char* label) {
    return std::format("{}##{}", displayName, label);
}
