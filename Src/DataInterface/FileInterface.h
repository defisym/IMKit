#pragma once

#include <functional>

#include "DataInterface.h"
#include "InfDefinition.h"

// in millisecond
constexpr auto DEFAULT_LOG_INTERVAL = 600000; // 10 minutes

// in byte
constexpr size_t GetByteCount(const size_t GB) { return GB * 1024Ui64 * 1024Ui64 * 1024Ui64; }
constexpr auto DEFAULT_CACHE_SIZE = GetByteCount(16Ui64); // 16GB
constexpr auto DEFAULT_LEFT_SIZE = GetByteCount(4Ui64); // 16GB

struct FileInterfaceConfig {
    FilePathConfig filePath = {}; // path to save file, default is "Log/"
    
    bool bWriteMetaData = false; // TODO not implemented
    size_t interval = DEFAULT_LOG_INTERVAL; // interval to write to disk
    size_t fileSizeThreshold = DEFAULT_CACHE_SIZE;
    size_t memoryLeftThreshold = DEFAULT_LEFT_SIZE;

    FileInterfaceConfig GetPathAppendConfig(const char* pSubPath) const;
};

template<>
struct std::hash<FileInterfaceConfig> {
    std::size_t operator()(FileInterfaceConfig const& s) const noexcept;
};

// log data to the disk
class FileInterface { // NOLINT(cppcoreguidelines-special-member-functions)
    FileInterfaceConfig config = {};
    std::string filePath;

    TimeStamp lastSaveTimeStamp = {};
    std::vector<StringifyCache> cache;

public:
    FileInterface(const FileInterfaceConfig& config = {});
    ~FileInterface();

    void UpdateConfig(const FileInterfaceConfig& config = {});
    // add data to internal 
    void AddData(const TimeStamp& timeStamp, const std::string& data);

private:
    std::string metaData;

public:
    // set metadata, which is written to the beginning of file
    void SetMetaData(const std::string& data) { metaData = data; }
    
private:
    struct FileWriter :FileBase {
        static constexpr size_t WRITE_SIZE_THRESHOLD = 16;

        ~FileWriter() override { CloseFile(); }
        
        // create new tempfile
        bool NewFile(const std::string& basePath, const std::string& name) override;
        
        template<typename T>
        size_t writeElement(FILE* fp, const T& element) {
            return fwrite(&element, sizeof(T), 1, fp);
        };
        size_t writeString(FILE* fp, const std::string& str) {
            return fwrite(str.data(), str.size(), 1, fp);
        };
        size_t writeString(FILE* fp, const char* pStr, const size_t sz) {
            return fwrite(pStr, sz, 1, fp);
        };

        // write metadata to tempfile
        void WriteMetaData(const std::string& metaData);
        // write data to tempfile
		// to optimize the IO performance
        // if cache size is smaller than WRITE_SIZE_THRESHOLD, do nothing
		// use bIgnoreThreshold = true to override this behavior
        // each element is saved in format of:
        //  - timeStampFormatted
        //  - FILEINF_NEWLINE
        //  - ElementFormatted
        //  - FILEINF_NEWLINE
        //  - FILEINF_NEWLINE
        void WriteFile(std::vector<StringifyCache>& cache, bool bIgnoreThreshold = false);
        // close and rename tempfile
        bool CloseFile() override;
    };

    FileWriter fileWriter = {};

public:
    // write element count to temp file
    // rename temp file in the format of start time ~ end time
    // return true if file saved correctly
    bool SaveData();
    // similiar as above, but check interval
    bool SaveDataWhenNeeded();
};