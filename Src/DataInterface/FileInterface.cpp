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
    cacheSize += cache.back().GetSize();
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

    const auto fileNameBase = cache.front().timeStampFormatted
        + " ~ "
        + cache.back().timeStampFormatted;

    const auto dataFileName = fileNameBase + ".data";
    const auto mapFileName = fileNameBase + ".map";

    namespace fs = std::filesystem;
    const auto dataPath = fs::path{ filePath } / dataFileName.c_str();
    const auto mapPath = fs::path{ filePath } / mapFileName.c_str();

    errno_t err = 0;

    FILE* datafp = nullptr;
    err = _wfopen_s(&datafp, dataPath.c_str(), L"wb");
    if (err != 0 || datafp == nullptr) { return false; }

    FILE* mapfp = nullptr;
    err = _wfopen_s(&mapfp, mapPath.c_str(), L"wb");
    if (err != 0 || mapfp == nullptr) { return false; }

    size_t elementCount = 0u;
    auto writeElement = []<typename T> (FILE* fp, const T& element) {
        return fwrite(&element, sizeof(T), 1, fp);
        };
    auto writeString = [] (FILE* fp, const std::string& str) {
        return fwrite(str.data(), str.size(), 1, fp);
        };

    if (!metaData.empty()) {
        // write meta data
        elementCount += writeElement(mapfp, metaData.size());
        elementCount += writeString(mapfp,metaData);
    }

    // jump table
    elementCount += writeElement(mapfp, cache.size());

    for (auto& it : cache) {
        // save jump table
        fpos_t curPos = 0;
        if (fgetpos(datafp, &curPos) != 0) { return false; }
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
    }

    cache.clear();
    cacheSize = 0u;

    return fclose(datafp) == 0 && fclose(mapfp) == 0;
}

bool FileInterface::SaveDataWhenNeeded() {
    // ------------------------------------------------
    // Condition
    // ------------------------------------------------

    // ------------------------------------
    // Definition
    // ------------------------------------
    auto conditionMemoryCache = [this] ()->bool {
        return cacheSize > config.memoryCacheThreshold;
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
        // memory has higher priority than time stamp
        if (conditionMemoryCache()) { break; }
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
