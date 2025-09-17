#pragma once

#include <chrono>
#include <vector>

#include "InfDefinition.h"

struct ParseInterfaceConfig {
    FilePathConfig filePath = {}; // path to save file, default is "Log/"

    bool bReadMetaData = false; // TODO not implemented
};

template<>
struct std::hash<ParseInterfaceConfig> {
    std::size_t operator()(ParseInterfaceConfig const& s) const noexcept;
};

// parse data from the disk
class ParseInterface { // NOLINT(cppcoreguidelines-special-member-functions)
    ParseInterfaceConfig config = {};
    std::string filePath;

    std::vector<StringifyCache> cache;

public:
    ParseInterface(const ParseInterfaceConfig& config = {});
    ~ParseInterface();

    void UpdateConfig(const ParseInterfaceConfig& config = {});
    const std::vector<StringifyCache>& GetData() { return cache; }

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
        // each element is saved in format of:
        //  - timeStampFormatted
        //  - FILEINF_NEWLINE
        //  - ElementFormatted
        //  - FILEINF_NEWLINE
        //  - FILEINF_NEWLINE
        void ReadFile(std::vector<StringifyCache>& cache);
        // close file
        bool CloseFile() override;
    };

    FileReader fileReader = {};

public:
    // read file from disk
    // name is the file name without path and suffix
    // then parse data element to internal cache
    // return true if file load correctly
    bool ReadData(const std::string& name);
};