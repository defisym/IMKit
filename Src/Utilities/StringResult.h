#pragma once

#include <string>

struct StringResult {
    std::string result;

    StringResult(const char* pLabel);
    StringResult(const std::string& label);
    StringResult(std::string&& label);
    operator const char* () const;
};
