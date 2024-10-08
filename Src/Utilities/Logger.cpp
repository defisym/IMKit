#include "Logger.h"

#include <filesystem>

namespace fs = std::filesystem;

Logger::Logger(Ctx* pCtx, const LoggerConfig& config) {
    this->pCtx = pCtx;
    this->config = config;

    // get relative path
    filePath.resize(MAX_PATH);
    GetFullPathNameA(config.filePath.c_str(), MAX_PATH, filePath.data(), nullptr);

    const auto path = fs::path{ filePath };

    std::error_code ec;
    fs::create_directory(path, ec);

    bValid = ec.value() == 0;
}

bool Logger::AddData(const LogData* pLogData) {

    return true;
}
