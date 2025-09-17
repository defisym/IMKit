#include "Logger.h"

#include "TimeStampHelper.h"
#include "Utilities/Param.h"

std::size_t std::hash<LoggerConfig>::operator()(LoggerConfig const& s) const noexcept {
    return GetParamHash(s);
}

void Logger::Clear() {
    lines.clear();
}

void Logger::AddLog(const char* pLog) {
    lines.emplace_back(pLog);
}

void Logger::AddLog(const std::string& log) {
    lines.emplace_back(log);
}

void Logger::AddLog(const std::string&& log) {
    lines.emplace_back(log);
}

void Logger::AddLog(const char* pType, const char* pLog) {
    AddLog(std::format("[{}] {}: {}",
        GetFormattedTimeStamp(),
        pType, pLog));
}

void Logger::AddLog(const std::string& type, const std::string& log) {
    AddLog(std::format("[{}] {}: {}",
        GetFormattedTimeStamp(),
        type, log));
}
