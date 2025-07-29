#pragma once

#include <string>
#include <chrono>

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

using TimeStamp = decltype(std::chrono::system_clock::now());

struct StringifyCache {
    TimeStamp timeStamp = {};
    std::string timeStampFormatted = {};
    std::string data;

    size_t GetSize() const {
        return sizeof(std::decay_t<std::remove_pointer_t<decltype(this)>>)
            + sizeof(char) * timeStampFormatted.length()
            + sizeof(char) * data.length();
    }
};
