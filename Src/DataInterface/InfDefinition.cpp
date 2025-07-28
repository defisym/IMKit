#include "InfDefinition.h"

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
