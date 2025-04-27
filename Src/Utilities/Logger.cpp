#include "Logger.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>

#include <Compress/zlibInterface.h>

#include "TimeStampHelper.h"
#include "GUIContext/Param/Param.h"
#include "IMGuiEx/I18NInterface.h"

namespace fs = std::filesystem;

std::size_t std::hash<LogDataConfig>::operator()(LogDataConfig const& s) const noexcept {
    return GetParamHash(s);
}

const std::string& LogDataInterface::Compress(const std::string& str) {
    // safe to pass reference
    if (!config.bCompress) {
        compressed = str;
        return compressed;
    }

    const auto sz = compressBound(static_cast<uLong>(str.size()));
    compressed.clear();
    compressed.resize(sz, '\0');

    uLong compressedSize = sz;

    const auto ret = compress(reinterpret_cast<Bytef*>(compressed.data()),
        &compressedSize,
        reinterpret_cast<const Bytef*>(str.data()),
        static_cast<uLong>(str.size()));

    if (ret != Z_OK) { compressedSize = 0; }
    compressed.resize(compressedSize);

    return compressed;
}

StringResult LogDataInterface::DataTypeInfo() {
    return I18N("General");
}

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

//void Logger::AddLog(LogDataInterface* pLogData) {
//    AddLog(std::format("[{}] {}: {}",
//        GetFormattedTimeStamp(),
//        pLogData->DataTypeInfo(),
//        pLogData->ToString()));
//}

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
