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
    struct FileReader {
        bool bFileOpen = false;
        std::string filePath = {};

        FILE* datafp = nullptr;
        FILE* mapfp = nullptr;

        size_t fileSize = 0u;
        size_t elementCount = 0u;
        size_t totalCacheSize = 0u;
        std::string startTimeStamp = {};
        std::string endTimeStamp = {};

        ~FileReader() { CloseFile(); }

        // create new tempfile
        bool NewFile(const std::string& fileName);

        template<typename T>
        size_t readElement(FILE* fp, const T& element) {
            return fwrite(&element, sizeof(T), 1, fp);
        };
        size_t readString(FILE* fp, const std::string& str) {
            return fwrite(str.data(), str.size(), 1, fp);
        };

        // write metadata to tempfile
        void ReadMetaData(const std::string& metaData);
        // write data to tempfile
        void ReadFile(std::vector<StringifyCache>& cache);
        // close and rename tempfile
        bool CloseFile();
    };

    FileReader fileReader = {};
};