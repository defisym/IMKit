#include "Logger.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>

#include <Compress/zlibInterface.h>

#include "GUIContext/Param/Param.h"

namespace fs = std::filesystem;

std::size_t std::hash<LogDataConfig>::operator()(LogDataConfig const& s) const noexcept {
    return GetParamHash(s);
}

const std::string& LogDataInterface::Compress(const std::string& str) {
    // safe to pass reference
    if (!config.bCompress) { return str; }

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

std::size_t std::hash<LoggerConfig>::operator()(LoggerConfig const& s) const noexcept {
    std::size_t hash = 0xcbf29ce484222325; // FNV-1a
    hash ^= std::hash<size_t>{}(s.interval);
    hash *= 0x100000001b3;  // FNV-1a

    for (char index : s.filePath) {
        hash ^= std::hash<char>{}(index);
        hash *= 0x100000001b3;  // FNV-1a
    }

    return hash;
}

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

        std::error_code ec = {};
        fs::create_directories(path, ec);

        if (ec.value() != 0) { break; }

        bValid = true;

        return;
    } while (false);

    bValid = false;   
}

Logger::~Logger() { SaveData(); }

using namespace std::chrono_literals;

void Logger::AddData(LogDataInterface* pLogData) {
    // update timestamp
    const auto currentTimeStamp = std::chrono::system_clock::now();

    // update cache
    cache.emplace_back(currentTimeStamp,
        GetFormattedTimeStamp(currentTimeStamp),
        pLogData->ToString());
}

bool Logger::SaveData() {
    if (!bValid) { return false; }
    if (cache.empty()) { return false; }

    const auto fileName = cache.front().timeStampFormatted
        + " ~ "
        + cache.back().timeStampFormatted
        + ".data";

    const auto path = fs::path{ filePath } / fileName.c_str();

    FILE* fp = nullptr;

    const auto err = _wfopen_s(&fp, path.c_str(), L"wb");
    if (err != 0 || fp == nullptr) { return false; }

    size_t elementCount = 0u;
    auto writeString = [&] (const std::string& str) {
        return fwrite(str.data(), str.size(), 1, fp);
        };

    if (metaDataCb != nullptr) {
        // write meta data
        elementCount += writeString("MetaData: ");
        elementCount += writeString(metaDataCb());
        elementCount += writeString("\r\n");
    }

    // write dummy jump table
    fpos_t jumpTableStart = 0;
    if (fgetpos(fp, &jumpTableStart) != 0) { return false; }

    constexpr size_t filePosition = 0;
    elementCount += fwrite(&filePosition, sizeof(size_t), cache.size(), fp);

    // write cache
    std::vector<fpos_t> jumpTable;

    for (auto& it : cache) {
        // save jump table
        fpos_t curPos = 0;
        if (fgetpos(fp, &curPos) != 0) { return false; }
        jumpTable.emplace_back(curPos);

        // save cache size
        const auto cacheSize = it.timeStampFormatted.length()
            + it.data.length()
            + 3 * strlen("\r\n");
        elementCount += fwrite(&cacheSize, sizeof(size_t), 1, fp);
        elementCount += writeString("\r\n");

        elementCount += writeString(it.timeStampFormatted);
        elementCount += writeString("\r\n");
        elementCount += writeString(it.data);
        elementCount += writeString("\r\n");
    }

    // write jump table
    if (fsetpos(fp, &jumpTableStart) != 0) { return false; }
    for (auto& it : jumpTable) {
        elementCount += fwrite(&it, sizeof(it), 1, fp);
    }

    cache.clear();

    const auto ret = fclose(fp);

    return ret == 0;
}

bool Logger::SaveDataWhenNeeded() {
    // update timestamp
    const auto currentTimeStamp = std::chrono::system_clock::now();
    if (lastSaveTimeStamp == TimeStamp{}) [[unlikely]] { lastSaveTimeStamp = currentTimeStamp; }
    const size_t interval = (currentTimeStamp - lastSaveTimeStamp) / 1ms;

    // check interval
    if (interval < config.interval) { return false; }
    lastSaveTimeStamp = currentTimeStamp;

    return SaveData();
}

void Logger::UpdateInterval(const size_t interval) {
    config.interval = interval;
}

std::string Logger::GetFormattedTimeStamp(const TimeStamp timeStamp, char const* pFmt) {
    std::tm time = {};
    const auto t = std::chrono::system_clock::to_time_t(timeStamp);
    [[maybe_unused]] const auto err = localtime_s(&time, &t);

    // https://stackoverflow.com/questions/28977585/how-to-get-put-time-into-a-variable
    auto timeString = std::string(MAX_PATH, 0);
    const auto sz = std::strftime(timeString.data(), timeString.size(), pFmt, &time);
    timeString.resize(sz);

    return timeString;
}
