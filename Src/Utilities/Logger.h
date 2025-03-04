#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <functional>

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
    [[nodiscard]] virtual const char* DataTypeInfo();

    using TimeStamp = decltype(std::chrono::system_clock::now());
};

struct LoggerConfig {
    bool bAutoScroll = true;  // Keep scrolling if already at the bottom.
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
    void AddLog(LogDataInterface* pLogData);
    void AddLog(const char* pType, const char* pLog);
    void AddLog(const std::string& type, const std::string& log);
};

constexpr auto DEFAULT_LOG_INTERVAL = 1000;
constexpr auto FILEPATH_LENGTH = 512;

struct FileLoggerConfig {
    // interval to write to disk
    size_t interval = DEFAULT_LOG_INTERVAL;
    char filePath[FILEPATH_LENGTH] = "Log/";
};

template<>
struct std::hash<FileLoggerConfig> {
    std::size_t operator()(FileLoggerConfig const& s) const noexcept;
};

struct Ctx;

// log data to the disk
class FileLogger { // NOLINT(cppcoreguidelines-special-member-functions)
    FileLoggerConfig config = {};

    bool bValid = false;
    std::string filePath;

    using TimeStamp = LogDataInterface::TimeStamp;
    TimeStamp lastSaveTimeStamp = {};

    struct CacheData {
        TimeStamp timeStamp = {};
        std::string timeStampFormatted = {};
        std::string data;
    };

    std::vector<CacheData> cache;

public:
    FileLogger(const FileLoggerConfig& config = {});
    ~FileLogger();

    void UpdateInterval(const size_t interval);
    // add data to internal cache
    void AddData(LogDataInterface* pLogData);

    using MetaDataCb = std::function<const std::string* ()>;
    MetaDataCb metaDataCb = nullptr;
    // add metadata, which is written to the beginning of file
    void AddMetaData(const MetaDataCb& cb) { metaDataCb = cb; }
    // write cache to disk
    // return true if file saved
    bool SaveData();
    // similiar as above, but check interval
    bool SaveDataWhenNeeded();
};

template<typename DataInterface>
concept ValidDataInterface = requires(DataInterface device) {
    std::is_base_of_v<LogDataInterface, DataInterface>;
    device.GetData();
    device.UpdateData(std::decay_t<decltype(device.GetData())>{});
};

template<ValidDataInterface DataInterface>
struct FileLoggerHelper {
    FileLogger logger;
    DataInterface dataInterface;

    FileLoggerHelper(const FileLoggerConfig& loggerConf = {},
        const LogDataConfig& logDataConf = {})
        :logger(loggerConf), dataInterface(logDataConf) {}

private:
    using DataType = std::decay_t<decltype(dataInterface.GetData())>;
    void UpdateData(const DataType& data) { dataInterface.UpdateData(data); }
    void AddData() { logger.AddData(&dataInterface); }

public:
    // ------------------------
    // serialization
    // ------------------------
    void AddData(const DataType& data) { UpdateData(data); AddData(); }
    void AddMetaData(const FileLogger::MetaDataCb& cb) { logger.AddMetaData(cb); }

    // ------------------------
    // save to file
    // ------------------------
    bool SaveData() { return logger.SaveData(); }
    bool SaveDataWhenNeeded() { return logger.SaveDataWhenNeeded(); }
};