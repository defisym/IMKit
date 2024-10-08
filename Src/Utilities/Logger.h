#pragma once

#include <string>
#include <chrono>

#include "../GUIContext/Context.h"

struct LogDataConfig {
    // save file in binary, otherwise is human-readable
    bool bBinary = false;
    // compress data, only works in binary mode
    bool bCompress = false;
};

class LogData {
    LogDataConfig config = {};

public:
    LogData(const LogDataConfig& config = {}) { this->config = config; }
    virtual ~LogData() = default;
    // save data to given buffer
    // if pBuffer == nullptr, return the size it requires
    virtual std::string ToString() const = 0;
};

struct LoggerConfig {
    size_t interval = 1000;
    std::string filePath = "Log/";
};

class Logger {
    // interval to write to disk
    Ctx* pCtx = nullptr;
    LoggerConfig config = {};

    bool bValid = false;
    std::string filePath;

    using TimeStamp = decltype(std::chrono::system_clock::now());
    TimeStamp lastSaveTimeStamp = {};

    struct CacheData {
        TimeStamp timeStamp = {};
        std::string timeStampFormatted = {};
        std::string data;
    };

    std::vector<CacheData> cache;

public:
    Logger(Ctx* pCtx, const LoggerConfig& config = {});

    // return true if file saved in this call
    bool AddData(const LogData* pLogData);
    static std::string GetFormattedTimeStamp(const TimeStamp timeStamp, char const* pFmt = "%Y-%m-%d %H-%M-%S");
};
