#include "Logger.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

Logger::Logger(const LoggerConfig& config) {
    this->config = config;
    
    do {
        // get relative path
        char fullPathName[MAX_PATH] = {};
        GetFullPathNameA(config.filePath, MAX_PATH, fullPathName, nullptr);
        filePath = fullPathName;

        // GetFullPathName will normalize / and \\ to \\
        // if not end with \\ , append it
        if (!filePath.ends_with('\\')) { filePath += '\\'; }

        // create dir
        const auto path = fs::path{ filePath };

        std::error_code ec;
        fs::create_directory(path, ec);

        if (ec.value() != 0) { break; }

        bValid = true;

        return;
    } while (false);

    bValid = false;   
}

using namespace std::chrono_literals;

bool Logger::AddData(LogData* pLogData) {
    // update timestamp
    const auto currentTimeStamp = std::chrono::system_clock::now();
    if (lastSaveTimeStamp == TimeStamp{}) [[unlikely]] { lastSaveTimeStamp = currentTimeStamp; }
    const size_t interval = (currentTimeStamp - lastSaveTimeStamp) / 1ms;

    // update cache
    cache.emplace_back(currentTimeStamp,
        GetFormattedTimeStamp(currentTimeStamp),
        pLogData->ToString());

    // check interval
    if (interval < config.interval) { return false; }
    lastSaveTimeStamp = currentTimeStamp;

    // save file
    const auto fileName = cache.front().timeStampFormatted
        + " ~ "
        + cache.back().timeStampFormatted
        + ".data";

    const auto path = fs::path{ filePath } / fileName.c_str();

    FILE* fp = nullptr;

    const auto err = _wfopen_s(&fp, path.c_str(), L"wb");
    if (err != 0 || fp == nullptr) { return false; }

    for (auto& it : cache) {
        auto writeString = [&] (const std::string& str) {
            return fwrite(str.data(), str.size(), 1, fp);
        };

        size_t elementCount = 0u;

        elementCount += writeString(it.timeStampFormatted);
        elementCount += writeString("\r\n");
        elementCount += writeString(it.data);
        elementCount += writeString("\r\n");
    }

    cache.clear();

    const auto ret = fclose(fp);

    return ret == 0;
}

void Logger::UpdateInterval(const size_t interval) {
    config.interval = interval;
}

std::string Logger::GetFormattedTimeStamp(const TimeStamp timeStamp, char const* pFmt) {
    std::tm time = {};
    const auto t = std::chrono::system_clock::to_time_t(timeStamp);
    const auto err = localtime_s(&time, &t);

    // https://stackoverflow.com/questions/28977585/how-to-get-put-time-into-a-variable
    auto timeString = std::string(MAX_PATH, 0);
    const auto sz = std::strftime(timeString.data(), timeString.size(), pFmt, &time);
    timeString.resize(sz);

    return timeString;
}
