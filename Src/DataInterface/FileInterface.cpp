#include "FileInterface.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>
#include <WindowsCommon.h>

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

void FileInterface::AddData(const TimeStamp& timeStamp, const std::string& data) {
    cache.emplace_back(timeStamp,
        GetFormattedTimeStamp(timeStamp),
        data);

    if (!fileWriter.bFileOpen) { fileWriter.NewFile(config.filePath); }
    fileWriter.WriteFile(cache);
    SaveDataWhenNeeded();
}

bool FileInterface::FileWriter::NewFile(const std::string& basePath, const std::string& name) {
    // ------------------------------------------------
    // Reset
    // ------------------------------------------------
    
    if (bFileOpen) { CloseFile(); }

    // ------------------------------------------------
    // Create folder to save files
    // ------------------------------------------------
    
    // get relative path
    char fullPathName[MAX_PATH] = {};
    GetFullPathNameA(basePath.c_str(), MAX_PATH, fullPathName, nullptr);
    filePath = fullPathName;

    // GetFullPathName will normalize / and \\ to \\
        // if not end with \\ , append it
    if (!filePath.ends_with('\\')) { filePath += '\\'; }

    // create dir
    namespace fs = std::filesystem;
    const auto path = fs::path{ filePath };

    std::error_code ec = {};
    fs::create_directories(path, ec);

    if (ec.value() != 0) { return false; }

    // ------------------------------------------------
    // Open file
    // ------------------------------------------------

    dataTempFileName = name + ".data";
    mapTempFileName = name + ".map";

    namespace fs = std::filesystem;
    const auto dataPath = fs::path{ filePath } / dataTempFileName.c_str();
    const auto mapPath = fs::path{ filePath } / mapTempFileName.c_str();

    errno_t err = 0;

    datafp = nullptr;
    err = _wfopen_s(&datafp, dataPath.c_str(), L"wb");
    if (err != 0 || datafp == nullptr) { return false; }

    mapfp = nullptr;
    err = _wfopen_s(&mapfp, mapPath.c_str(), L"wb");
    if (err != 0 || mapfp == nullptr) { return false; }

    // jump table: write dummy size
    elementCount += writeElement(mapfp, 0);

    bFileOpen = true;

    return true;
}

void FileInterface::FileWriter::WriteMetaData(const std::string& metaData) {
    if (!bFileOpen) { return; }
    if (metaData.empty()) { return; }

    // write meta data
    elementCount += writeElement(mapfp, metaData.size());
    elementCount += writeString(mapfp, metaData);
}

void FileInterface::FileWriter::WriteFile(std::vector<CacheData>& cache) {
    if (!bFileOpen) { return; }
    if (cache.size() < WRITE_SIZE_THRESHOLD) { return; }
    
    if (startTimeStamp.empty()) { startTimeStamp = cache.front().timeStampFormatted; }
    endTimeStamp = cache.back().timeStampFormatted;
    totalCacheSize += cache.size();

    for (auto& it : cache) {
        // save jump table
        fpos_t curPos = 0;
        if (fgetpos(datafp, &curPos) != 0) { break; }
        elementCount += writeElement(mapfp, curPos); // pos

        // save cache size
        const auto cacheSize = it.timeStampFormatted.length()
            + it.data.length()
            + 3 * strlen("\r\n");
        elementCount += writeString(datafp, it.timeStampFormatted);
        elementCount += writeString(datafp, "\r\n");
        elementCount += writeString(datafp, it.data);
        elementCount += writeString(datafp, "\r\n");
        elementCount += writeString(datafp, "\r\n");

        fileSize += it.GetSize();
    }

    cache.clear();

    return;
}

bool FileInterface::FileWriter::CloseFile() {
    if (!bFileOpen) { return true; }
    if (datafp == nullptr || mapfp == nullptr) { return true; }

    do {
        // write size
        if (fseek(mapfp, 0, SEEK_SET) != 0) { break; }
        elementCount += writeElement(mapfp, totalCacheSize);

        // close file
        if (fclose(datafp) != 0) { break; }
        datafp = nullptr;
        if (fclose(mapfp) != 0) { break; }
        mapfp = nullptr;

        // rename
        const auto name = startTimeStamp + " ~ " + endTimeStamp;
        const auto dataFileName = name + ".data";
        const auto mapFileName = name + ".map";

        namespace fs = std::filesystem;
        const auto dataOldPath = fs::path{ filePath } / dataTempFileName.c_str();
        const auto dataNewPath = fs::path{ filePath } / dataFileName.c_str();
        const auto mapOldPath = fs::path{ filePath } / mapTempFileName.c_str();
        const auto mapNewPath = fs::path{ filePath } / mapFileName.c_str();

        fs::rename(dataOldPath, dataNewPath);
        fs::rename(mapOldPath, mapNewPath);
    } while (false);

    // reset
    bFileOpen = false;
    filePath = {}; dataTempFileName = {}; mapTempFileName = {};
    fileSize = 0u; elementCount = 0u; totalCacheSize = 0u;
    startTimeStamp = {}; endTimeStamp = {};

    return true;
}

bool FileInterface::SaveData() {
    return fileWriter.CloseFile();
}

bool FileInterface::SaveDataWhenNeeded() {
    // ------------------------------------------------
    // Condition
    // ------------------------------------------------

    // ------------------------------------
    // Definition
    // ------------------------------------
    auto conditionFileSize = [this] ()->bool {
        return fileWriter.fileSize > config.fileSizeThreshold;
        };
    auto conditionMemoryLeft = [this] ()->bool {
        return GetSystemMemoryInfo() < config.memoryLeftThreshold;
        };
    auto conditionTimeStamp = [this] ()->bool {
        using namespace std::chrono_literals;

        // update timestamp
        const auto currentTimeStamp = std::chrono::system_clock::now();
        if (lastSaveTimeStamp == TimeStamp{}) [[unlikely]] { lastSaveTimeStamp = currentTimeStamp; }
        const size_t interval = (currentTimeStamp - lastSaveTimeStamp) / 1ms;

        // check interval
        return interval > config.interval;
        };

    // ------------------------------------
    // Process
    // ------------------------------------

    do {
        if (conditionFileSize()) { break; }
        if (conditionMemoryLeft()) { break; }
        if (conditionTimeStamp()) { break; }

        return false;
    } while (false);
    
    // ------------------------------------------------
    // Update
    // ------------------------------------------------
    
    // ------------------------------------
    // Definition
    // ------------------------------------
    
    auto updateTimeStamp = [this] () {
        lastSaveTimeStamp = std::chrono::system_clock::now();
        };

    // ------------------------------------
    // Process
    // ------------------------------------
    
    updateTimeStamp();

    // ------------------------------------------------
    // Save data
    // ------------------------------------------------
    
    return SaveData();
}

