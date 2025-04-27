#pragma once

#include <string>
#include <vector>

struct LoggerConfig {
    bool bAutoScroll = false;  // Keep scrolling if already at the bottom.
};

template<>
struct std::hash<LoggerConfig> {
    std::size_t operator()(LoggerConfig const& s) const noexcept;
};

struct Logger {
    std::vector<std::string> lines;

    void Clear();

    // directly add
    void AddLog(const char* pLog);
    void AddLog(const std::string& log);
    void AddLog(const std::string&& log);
    
    // with timestamp
    void AddLog(const char* pType, const char* pLog);
    void AddLog(const std::string& type, const std::string& log);
};
