#pragma once

#include "../../../Src/DLL/include/OTDR.h"

#include <string>
#include <chrono>

struct LogDataConfig {
    // save file in binary, otherwise is human-readable
    bool bBinary = false;
    // compress data, only works in binary mode
    bool bCompress = false;
};

class LogData {
protected:
    LogDataConfig config = {};

public:
    LogData(const LogDataConfig& config = {}) { LogData::UpdateConfig(config); }
    virtual ~LogData() = default;

    void UpdateConfig(const LogDataConfig& conf = {}) { this->config = config; }
    [[nodiscard]] virtual const std::string& ToString() = 0;
};

struct LoggerConfig {
    size_t interval = 1000;
    std::string filePath = "Log/";
};

struct Ctx;

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
    bool AddData(LogData* pLogData);
    static std::string GetFormattedTimeStamp(const TimeStamp timeStamp, char const* pFmt = "%Y-%m-%d %H-%M-%S");
};

template<typename Data>
    requires std::is_base_of_v<LogData, Data>
struct LoggerHelper {
    Logger logger;
    Data loggerData;

    LoggerHelper(const LoggerConfig& loggerConf = {},
        const LogDataConfig& logDataConf = {})
        :logger(loggerConf), loggerData(logDataConf) {}
    bool AddData() { return logger.AddData(&loggerData); }
};