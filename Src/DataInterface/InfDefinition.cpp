#include "InfDefinition.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <WindowsCommon.h>

#include <string.h>
#include <filesystem>

#include "GUIContext/Param/Param.h"

void FilePathConfig::AppendPath(const char* pSubPath) {
    [[maybe_unused]]const auto err = strcat_s(filePath, pSubPath);
}

FilePathConfig FilePathConfig::GetPathAppendConfig(const char* pSubPath) const {
    FilePathConfig ret = *this;
    ret.AppendPath(pSubPath);

    return ret;
}

std::size_t std::hash<FilePathConfig>::operator()(FilePathConfig const& s) const noexcept {
    std::size_t hash = 0xcbf29ce484222325; // FNV-1a
    hash ^= GetStringHash(s.filePath);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

std::string GetAbsolutePathName(const std::string& basePath) {
    // get relative path
    char fullPathName[MAX_PATH] = {};
    GetFullPathNameA(basePath.c_str(), MAX_PATH, fullPathName, nullptr);
    std::string filePath = fullPathName;

    // GetFullPathName will normalize / and \\ to \\
    // if not end with \\ , append it
    if (!filePath.ends_with('\\')) { filePath += '\\'; }

    return filePath;
}

void FileBase::Reset() {
    bFileOpen = false;
    filePath = {}; dataFileName = {}; mapFileName = {};
    fileSize = 0u; elementCount = 0u; totalCacheSize = 0u;
    startTimeStamp = {}; endTimeStamp = {};
}

bool FileBase::FileOpen() const {
    if (!bFileOpen) { return false; }
    if (datafp == nullptr || mapfp == nullptr) { return false; }

    return true;
}

bool FileBase::InitializeFile(const std::string& basePath, const std::string& name,
    const wchar_t* pMode) {
    // ------------------------------------------------
       // Create folder for save files
       // ------------------------------------------------
    filePath = GetAbsolutePathName(basePath);

    // create dir
    namespace fs = std::filesystem;
    const auto path = fs::path{ filePath };

    std::error_code ec = {};
    fs::create_directories(path, ec);

    if (ec.value() != 0) { return false; }

    // ------------------------------------------------
    // Open file
    // ------------------------------------------------

    dataFileName = name + ".data";
    mapFileName = name + ".map";

    namespace fs = std::filesystem;
    const auto dataPath = fs::path{ filePath } / dataFileName.c_str();
    const auto mapPath = fs::path{ filePath } / mapFileName.c_str();

    errno_t err = 0;

    datafp = nullptr;
    err = _wfopen_s(&datafp, dataPath.c_str(), pMode);
    if (err != 0 || datafp == nullptr) { return false; }

    mapfp = nullptr;
    err = _wfopen_s(&mapfp, mapPath.c_str(), pMode);
    if (err != 0 || mapfp == nullptr) { return false; }

    bFileOpen = true;

    return true;
}

bool FileBase::NewFile(const std::string& basePath, const std::string& name) {
    return InitializeFile(basePath, name, L"wb"); 
}

bool FileBase::OpenFile(const std::string& basePath, const std::string& name) {
    return InitializeFile(basePath, name, L"rb");
}

bool FileBase::CloseFile() {
    do {
        if (fclose(datafp) != 0) { break; }
        datafp = nullptr;
        if (fclose(mapfp) != 0) { break; }
        mapfp = nullptr;

        return true;
    } while (false);

    return false;
}
