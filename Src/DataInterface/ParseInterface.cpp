#include "ParseInterface.h"

std::size_t std::hash<ParseInferfaceConfig>::operator()(ParseInferfaceConfig const& s) const noexcept {
    std::size_t hash = 0xcbf29ce484222325; // FNV-1a

    hash ^= std::hash<FilePathConfig>{}(s.filePath);
    hash *= 0x100000001b3;  // FNV-1a

    hash ^= std::hash<bool>{}(s.bReadMetaData);
    hash *= 0x100000001b3;  // FNV-1a

    return hash;
}

ParseInterface::ParseInterface(const ParseInferfaceConfig& config) {
    UpdateConfig();
}

ParseInterface::~ParseInterface() {}

void ParseInterface::UpdateConfig(const ParseInferfaceConfig& config) {
    this->config = config;
}

bool ParseInterface::ReadData() {
    return false;
}

bool ParseInterface::FileReader::OpenFile(const std::string& basePath, const std::string& name) {
    // ------------------------------------------------
    // Reset
    // ------------------------------------------------

    if (FileOpen()) { CloseFile(); }

    // ------------------------------------------------
    // Create folder & Open file
    // ------------------------------------------------

    if (!FileBase::OpenFile(basePath, name)) { return false; }

    return true;
}

void ParseInterface::FileReader::ReadMetaData(std::string& metaData) {
    if (!FileOpen()) { return; }

    // read meta data
    size_t sz = 0u;
    elementCount += readElement(mapfp, sz);
    elementCount += readString(mapfp, sz, metaData);
}

void ParseInterface::FileReader::ReadFile(std::vector<StringifyCache>& cache) {
    if (!FileOpen()) { return; }

    elementCount += readElement(mapfp, totalCacheSize);

    std::vector<size_t> offset = {};
    for(size_t index = 0u; index < totalCacheSize; index++) {
        size_t sz = 0u;
        elementCount += readElement(mapfp, sz);
        offset.push_back(sz);
    }

    for (decltype(offset.cbegin()) start = offset.cbegin(), next = start + 1;
        next != offset.cend();
        start++, next++) {
        StringifyCache item = {};

        const auto sz = *next - *start;
        elementCount += readString(datafp, sz, item.data);

        item.timeStampFormatted = item.data.substr(0, item.data.find_first_of(FILEINF_NEWLINE));
        item.data = item.data.substr(item.timeStampFormatted.length() + strlen(FILEINF_NEWLINE),
            item.data.length() - item.timeStampFormatted.length() - 3 * strlen(FILEINF_NEWLINE));

        cache.emplace_back(std::move(item));
    }

    return;
}

bool ParseInterface::FileReader::CloseFile() {
    if (!FileOpen()) { return true; }
    if (!FileBase::CloseFile()) { return false; }

    return true;
}