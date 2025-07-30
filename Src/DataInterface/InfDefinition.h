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

std::string GetAbsolutePathName(const std::string& basePath);

template<>
struct std::hash<FilePathConfig> {
    std::size_t operator()(FilePathConfig const& s) const noexcept;
};

struct FileBase {
    bool bFileOpen = false;
    std::string filePath = {};
    std::string dataFileName = {};
    std::string mapFileName = {};

    FILE* datafp = nullptr;
    FILE* mapfp = nullptr;

    size_t fileSize = 0u;
    size_t elementCount = 0u;
    size_t totalCacheSize = 0u;
    std::string startTimeStamp = {};
    std::string endTimeStamp = {};

    virtual ~FileBase() = default;
    virtual void Reset();

    bool FileOpen() const;

private:
    bool InitializeFile(const std::string& basePath, const std::string& name,
        const wchar_t* pMode) final;
public:
    virtual bool NewFile(const std::string& basePath, const std::string& name);
    virtual bool OpenFile(const std::string& basePath, const std::string& name);

    virtual bool CloseFile();
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
