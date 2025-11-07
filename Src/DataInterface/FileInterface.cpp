#include "FileInterface.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <filesystem>
#include <WindowsCommon.h>

#include "Utilities/TimeStampHelper.h"
#include "IMGuiEx/I18NInterface.h"

FileInterfaceConfig FileInterfaceConfig::GetPathAppendConfig(const char* pSubPath) const {
    FileInterfaceConfig ret = *this;
    ret.filePath.AppendPath(pSubPath);

    return ret;
}

std::size_t std::hash<FileInterfaceConfig>::operator()(FileInterfaceConfig const& s) const noexcept {
    std::size_t hash = 0xcbf29ce484222325; // FNV-1a

    hash ^= std::hash<FilePathConfig>{}(s.filePath);
    hash *= 0x100000001b3;  // FNV-1a   

    hash ^= std::hash<bool>{}(s.bWriteMetaData);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<size_t>{}(s.interval);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<size_t>{}(s.fileSizeThreshold);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<size_t>{}(s.memoryLeftThreshold);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

FileInterface::FileInterface(const FileInterfaceConfig& config) {
    UpdateConfig(config);
}

FileInterface::~FileInterface() {
	// write all cache to disk when destruct
    WriteCache();
}

void FileInterface::UpdateConfig(const FileInterfaceConfig& config) {
    this->config = config;
}

void FileInterface::AddData(const TimeStamp& timeStamp, const std::string& data) {
    cache.emplace_back(timeStamp,
        GetFormattedTimeStamp(timeStamp),
        data);

    if (!fileWriter.bFileOpen) { 
        fileWriter.NewFile(config.filePath, "temp");
        fileWriter.WriteMetaData(metaData);
        fileWriter.WriteJumpTable();
    }

    fileWriter.WriteFile(cache);
    SaveDataWhenNeeded();
}

void FileInterface::WriteCache() {
    fileWriter.WriteFile(cache, true);
    SaveData();
}

bool FileInterface::FileWriter::NewFile(const std::string& basePath, const std::string& name) {
    // ------------------------------------------------
    // Reset
    // ------------------------------------------------
    
    if (FileOpen()) { CloseFile(); }

    // ------------------------------------------------
    // Create folder & Open file
    // ------------------------------------------------

    if (!FileBase::NewFile(basePath, name)) { return false; }

    return true;
}

void FileInterface::FileWriter::WriteMetaData(const std::string& metaData) {
    if (!FileOpen()) { return; }

	// write meta data, okay if metaData is empty
    elementCount += writeElement(mapfp, metaData.size());
    elementCount += writeString(mapfp, metaData);
}

void FileInterface::FileWriter::WriteJumpTable() {
    if (!FileOpen()) { return; }

    // jump table: write dummy size at mapfp start
    totalCacheSize = 0u;
    elementCount += writeElement(mapfp, totalCacheSize);
}

void FileInterface::FileWriter::WriteFile(std::vector<StringifyCache>& cache, bool bIgnoreThreshold) {
    if (!FileOpen()) { return; }
    if (!bIgnoreThreshold && cache.size() < WRITE_SIZE_THRESHOLD) { return; }
    
    if (startTimeStamp.empty()) { startTimeStamp = cache.front().timeStampFormatted; }
    endTimeStamp = cache.back().timeStampFormatted;
    totalCacheSize += static_cast<std::uint32_t>(cache.size());

    for (auto& it : cache) {
        // save jump table
        fpos_t curPos = 0;
        if (fgetpos(datafp, &curPos) != 0) { break; }
        elementCount += writeElement(mapfp, curPos); // pos

        // write cache
        elementCount += writeString(datafp, it.timeStampFormatted);
        elementCount += writeString(datafp, FILEINF_NEWLINE);
        elementCount += writeString(datafp, it.data);
        elementCount += writeString(datafp, FILEINF_NEWLINE);
        elementCount += writeString(datafp, FILEINF_NEWLINE);

        // update file size
        fileSize += it.timeStampFormatted.length()
            + it.data.length()
            + 3 * strlen(FILEINF_NEWLINE);
    }

    cache.clear();

    return;
}

bool FileInterface::FileWriter::CloseFile() {
    if (!FileOpen()) { return true; }
    if (startTimeStamp.empty()) { return true; } // nothing has been write

    do {
        // write size
        if (fseek(mapfp, 0, SEEK_SET) != 0) { break; }
        elementCount += writeElement(mapfp, totalCacheSize);

        // close file
        if (!FileBase::CloseFile()) { break; }

        // rename
        namespace fs = std::filesystem;
        const auto dataOldPath = fs::path{ filePath } / dataFileName.c_str();
        const auto mapOldPath = fs::path{ filePath } / mapFileName.c_str();

        const auto name = startTimeStamp + " ~ " + endTimeStamp;
        const auto dataNewPath = fs::path{ filePath } / (name + ".data").c_str();
        const auto mapNewPath = fs::path{ filePath } / (name + ".map").c_str();

        fs::rename(dataOldPath, dataNewPath);
        fs::rename(mapOldPath, mapNewPath);
    } while (false);

    // reset
    Reset();

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

