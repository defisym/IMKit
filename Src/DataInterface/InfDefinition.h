#pragma once

#include <string>

constexpr auto FILEPATH_LENGTH = 512;

struct FilePathConfig {
    char filePath[FILEPATH_LENGTH] = "Log/";

    operator const char*() const { return filePath; }
    operator const std::string () const { return filePath; }

    void AppendPath(const char* pSubPath);
    FilePathConfig GetPathAppendConfig(const char* pSubPath) const;
};

template<>
struct std::hash<FilePathConfig> {
    std::size_t operator()(FilePathConfig const& s) const noexcept;
};
