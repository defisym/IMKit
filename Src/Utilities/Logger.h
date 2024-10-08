#pragma once

#include <string>
#include <chrono>

#include "../GUIContext/Context.h"

struct LogData {
    virtual ~LogData();
    // save data to given buffer
    // if pBuffer == nullptr, return the size it requires
    virtual size_t SaveData(byte* pBuffer = nullptr) = 0;
};

struct LoggerConfig {
    // save file in binary, otherwise is human-readable
    bool bBinary = false;
    // compress data, only works in binary mode
    bool bCompress = false;
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
public:
    Logger(Ctx* pCtx, const LoggerConfig& config = {});
    bool AddData(const LogData* pLogData);
    static std::string GetFormattedTimeStamp(const TimeStamp timeStamp, char const* pFmt = "%Y-%m-%d %H-%M-%S");
};
