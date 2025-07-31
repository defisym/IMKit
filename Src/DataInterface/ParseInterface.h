#pragma once

#include <chrono>
#include <vector>

#include "InfDefinition.h"

struct ParseInferfaceConfig {
    FilePathConfig filePath = {}; // path to save file, default is "Log/"

    bool bReadMetaData = false; // TODO not implemented
};

template<>
struct std::hash<ParseInferfaceConfig> {
    std::size_t operator()(ParseInferfaceConfig const& s) const noexcept;
};

// parse data from the disk
class ParseInterface { // NOLINT(cppcoreguidelines-special-member-functions)
    ParseInferfaceConfig config = {};
    std::string filePath;

    std::vector<StringifyCache> cache;

public:
    ParseInterface(const ParseInferfaceConfig& config = {});
    ~ParseInterface();

    void UpdateConfig(const ParseInferfaceConfig& config = {});

private:
    struct FileReader :FileBase {
        ~FileReader() override { CloseFile(); }

        // create new tempfile
        bool OpenFile(const std::string& basePath, const std::string& name);

        template<typename T>
        size_t readElement(FILE* fp, T& element) {
            return fread(&element, sizeof(T), 1, fp);
        };
        size_t readString(FILE* fp, size_t length, std::string& str) {
            str.resize(length);
            return fread(str.data(), str.size(), 1, fp);
        };

        // read metadata to string
        void ReadMetaData(std::string& metaData);
        // read data to cache
        void ReadFile(std::vector<StringifyCache>& cache);
        // close file
        bool CloseFile() override;
    };

public: // temp for test, should be private
    FileReader fileReader = {};

public:
    // write cache to disk
    // return true if file saved
    bool ReadData();
};