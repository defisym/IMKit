#include "FileInterface.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>

#include "Utilities/TimeStampHelper.h"
#include "GUIContext/Param/Param.h"
#include "IMGuiEx/I18NInterface.h"

FileInterfaceConfig FileInterfaceConfig::GetPathAppendConfig(const char* pSubPath) const {
    FileInterfaceConfig ret = *this;
    const auto err = strcat_s(ret.filePath, pSubPath);

    return ret;
}

std::size_t std::hash<FileInterfaceConfig>::operator()(FileInterfaceConfig const& s) const noexcept {
    std::size_t hash = 0xcbf29ce484222325; // FNV-1a
    hash ^= std::hash<size_t>{}(s.interval);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= GetStringHash(s.filePath);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

FileInterface::FileInterface(const FileInterfaceConfig& config) {
    UpdateConfig();
}

FileInterface::~FileInterface() { SaveData(); }

void FileInterface::UpdateConfig(const FileInterfaceConfig& config) {
    this->config = config;
}

void FileInterface::AddData(const std::string& data) {
    // update timestamp
    const auto currentTimeStamp = std::chrono::system_clock::now();

    // update cache
    cache.emplace_back(currentTimeStamp,
        GetFormattedTimeStamp(currentTimeStamp),
        data);
}

bool FileInterface::CreateFolder() {
    // get relative path
    char fullPathName[MAX_PATH] = {};
    GetFullPathNameA(config.filePath, MAX_PATH, fullPathName, nullptr);
    filePath = fullPathName;

    // GetFullPathName will normalize / and \\ to \\
        // if not end with \\ , append it
    if (!filePath.ends_with('\\')) { filePath += '\\'; }

    // create dir
    namespace fs = std::filesystem;
    const auto path = fs::path{ filePath };

    std::error_code ec = {};
    fs::create_directories(path, ec);

    return ec.value() == 0;
}

bool FileInterface::SaveData() {
    if (!CreateFolder()) { return false; }
    if (cache.empty()) { return false; }

    const auto fileName = cache.front().timeStampFormatted
        + " ~ "
        + cache.back().timeStampFormatted
        + ".data";

    namespace fs = std::filesystem;
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
        elementCount += writeString("\r\n");
        elementCount += writeString(*metaDataCb());
        elementCount += writeString("\r\n");
    }

    // write dummy jump table
    elementCount += writeString("JumpTable: ");
    elementCount += writeString("\r\n");

    fpos_t jumpTableStart = 0;
    if (fgetpos(fp, &jumpTableStart) != 0) { return false; }

    constexpr size_t filePosition = 0;
    for (size_t index = 0; index < cache.size(); index++) {
        elementCount += fwrite(&filePosition, sizeof(size_t), 1, fp);
    }
    elementCount += writeString("\r\n");

    // data region
    elementCount += writeString("Data Region: ");
    elementCount += writeString("\r\n");

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

bool FileInterface::SaveDataWhenNeeded() {
    using namespace std::chrono_literals;

    // update timestamp
    const auto currentTimeStamp = std::chrono::system_clock::now();
    if (lastSaveTimeStamp == TimeStamp{}) [[unlikely]] { lastSaveTimeStamp = currentTimeStamp; }
    const size_t interval = (currentTimeStamp - lastSaveTimeStamp) / 1ms;

    // check interval
    if (interval < config.interval) { return false; }
    lastSaveTimeStamp = currentTimeStamp;

    return SaveData();
}
