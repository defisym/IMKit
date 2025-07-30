#include "InfDefinition.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <WindowsCommon.h>

#include <string.h>

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
