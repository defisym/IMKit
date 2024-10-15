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

class LogData {  // NOLINT(cppcoreguidelines-special-member-functions)
protected:
    LogDataConfig config = {};

public:
    LogData(const LogDataConfig& conf = {}) { LogData::UpdateConfig(conf); }
    virtual ~LogData() = default;

    void UpdateConfig(const LogDataConfig& conf = {}) { this->config = conf; }
    [[nodiscard]] virtual const std::string& ToString() = 0;
};

constexpr auto DEFAULT_LOG_INTERVAL = 1000;

struct LoggerConfig {
    // interval to write to disk
    size_t interval = DEFAULT_LOG_INTERVAL;
    std::string filePath = "Log/";
};

struct Ctx;

class Logger {
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
    Logger(const LoggerConfig& config = {});

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