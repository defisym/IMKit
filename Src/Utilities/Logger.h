#pragma once

#include <string>
#include <chrono>

struct LogDataConfig {
    // save file in binary, otherwise is human-readable
    bool bBinary = false;
    // compress data, only works in binary mode
    bool bCompress = false;
};

template<>
struct std::hash<LogDataConfig> {
    std::size_t operator()(LogDataConfig const& s) const noexcept;
};

// Defines how data should be logged
class LogDataInterface {  // NOLINT(cppcoreguidelines-special-member-functions)
protected:
    LogDataConfig config = {};
    std::string compressed;

public:
    LogDataInterface(const LogDataConfig& conf = {}) { UpdateConfig(conf); }
    virtual ~LogDataInterface() = default;

    void UpdateConfig(const LogDataConfig& conf = {}) { this->config = conf; }
    [[nodiscard]] const std::string& Compress(const std::string& str);
    [[nodiscard]] virtual const std::string& ToString() = 0;
};

constexpr auto DEFAULT_LOG_INTERVAL = 1000;
constexpr auto FILEPATH_LENGTH = 512;

struct LoggerConfig {
    // interval to write to disk
    size_t interval = DEFAULT_LOG_INTERVAL;
    char filePath[FILEPATH_LENGTH] = "Log/";
};

template<>
struct std::hash<LoggerConfig> {
    std::size_t operator()(LoggerConfig const& s) const noexcept;
};

struct Ctx;

// log data to the disk
class Logger { // NOLINT(cppcoreguidelines-special-member-functions)
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
    ~Logger();

    // add data to internal cache
    void AddData(LogDataInterface* pLogData);
    // write cache to disk
    // return true if file saved
    bool SaveData();
    // similiar as above, but check interval
    bool SaveDataWhenNeeded();

    void UpdateInterval(const size_t interval);
    static std::string GetFormattedTimeStamp(const TimeStamp timeStamp, char const* pFmt = "%Y-%m-%d %H-%M-%S");
};

template<typename DataInterface>
concept ValidDataInterface = requires(DataInterface device) {
    std::is_base_of_v<LogDataInterface, DataInterface>;
    device.GetData();
    device.UpdateData(std::decay_t<decltype(device.GetData())>{});
};

template<ValidDataInterface DataInterface>
struct LoggerHelper {
    Logger logger;
    DataInterface dataInterface;

    LoggerHelper(const LoggerConfig& loggerConf = {},
        const LogDataConfig& logDataConf = {})
        :logger(loggerConf), dataInterface(logDataConf) {}

private:
    using DataType = decltype(dataInterface.GetData());
    void UpdateData(const DataType& data) { dataInterface.UpdateData(data); }
    void AddData() { logger.AddData(&dataInterface); }

public:
    void AddData(const DataType& data) { UpdateData(data); AddData(); }
    bool SaveDataWhenNeeded() { return logger.SaveDataWhenNeeded(); }
};