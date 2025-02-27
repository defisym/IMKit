#include "StringResult.h"

StringResult::StringResult(const char* pLabel) {
    result = pLabel;
}

StringResult::StringResult(const std::string& label) {
    result = label;
}

StringResult::StringResult(std::string&& label) {
    result = std::move(label);
}

StringResult::operator const char* () const {
    return result.c_str();
}
