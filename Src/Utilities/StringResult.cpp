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

StringResult::operator const char* () const& {
    return result.c_str();
}

StringResult::operator const std::string& () const& {
    return result;
}

StringResult::operator std::string() const& {
    return result;
}

StringResult::operator std::string() && {
    return std::move(result);
}
