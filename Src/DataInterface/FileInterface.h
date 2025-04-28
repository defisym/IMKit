#pragma once

#include <chrono>
#include <functional>

#include "DataInterface.h"

// in millisecond
constexpr auto DEFAULT_LOG_INTERVAL = 600000; // 10 minutes
constexpr auto FILEPATH_LENGTH = 512;

struct FileInterfaceConfig {
    // interval to write to disk
    size_t interval = DEFAULT_LOG_INTERVAL;
    char filePath[FILEPATH_LENGTH] = "Log/";

    FileInterfaceConfig GetPathAppendConfig(const char* pSubPath) const;
};

template<>
struct std::hash<FileInterfaceConfig> {
    std::size_t operator()(FileInterfaceConfig const& s) const noexcept;
};

// log data to the disk
class FileInterface { // NOLINT(cppcoreguidelines-special-member-functions)
    FileInterfaceConfig config = {};
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
    FileInterface(const FileInterfaceConfig& config = {});
    ~FileInterface();

    void UpdateConfig(const FileInterfaceConfig& config = {});
    // add data to internal 
    void AddData(const std::string& data);

    std::string metaData;
    // set metadata, which is written to the beginning of file
    void SetMetaData(const std::string& data) { metaData = data; }

    // create folder to save files
    bool CreateFolder();
    // write cache to disk
    // return true if file saved
    bool SaveData();
    // similiar as above, but check interval
    bool SaveDataWhenNeeded();
};