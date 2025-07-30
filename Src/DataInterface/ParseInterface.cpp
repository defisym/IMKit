#include "ParseInterface.h"

ParseInterface::ParseInterface(const ParseInferfaceConfig& config) {
    UpdateConfig();
}

ParseInterface::~ParseInterface() {}

void ParseInterface::UpdateConfig(const ParseInferfaceConfig& config) {
    this->config = config;
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
}

void ParseInterface::FileReader::ReadFile(std::vector<StringifyCache>& cache) {
    if (!FileOpen()) { return; }

    size_t itemSz = 0u;
    elementCount += readElement(mapfp, itemSz);

    std::vector<size_t> offset = {};
    for(size_t index = 0u; index < itemSz; index++) {
        size_t sz = 0u;
        elementCount += readElement(mapfp, sz);
        offset.push_back(sz);
    }

    return;
}

bool ParseInterface::FileReader::CloseFile() {
    if (!FileOpen()) { return true; }
    if (!FileBase::CloseFile()) { return false; }

    return true;
}