#pragma once

#include <chrono>
#include <vector>

#include "InfDefinition.h"

using ParseInferfaceConfig = FilePathConfig;

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

    FileReader fileReader = {};
};