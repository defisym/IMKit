#include "ParseInterface.h"

ParseInterface::ParseInterface(const ParseInferfaceConfig& config) {
    UpdateConfig();
}

ParseInterface::~ParseInterface() {}

void ParseInterface::UpdateConfig(const ParseInferfaceConfig& config) {
    this->config = config;
}

bool ParseInterface::FileReader::NewFile(const std::string& fileName) {
    return false;
}

void ParseInterface::FileReader::ReadMetaData(const std::string& metaData) {
}

void ParseInterface::FileReader::ReadFile(std::vector<StringifyCache>& cache) {
}

bool ParseInterface::FileReader::CloseFile() {
    return false;
}