#pragma once

#include <string>
#include <format>

struct StringResult {
    std::string result;

    StringResult(const char* pLabel);
    StringResult(const std::string& label);
    StringResult(std::string&& label);

    operator const char* () const&;
    operator const std::string& () const&;

    const char* c_str() const;
};

template <>
struct std::formatter<StringResult>:std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const StringResult& obj, FormatContext& ctx) const {
        return std::formatter<std::string>::format(obj.result, ctx);
    }
};