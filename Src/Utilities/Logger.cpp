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

using namespace std::chrono_literals;
bool Logger::AddData(const LogData* pLogData) {
std::string Logger::GetFormattedTimeStamp(const TimeStamp timeStamp, char const* pFmt) {
    std::tm time = {};
    const auto t = std::chrono::system_clock::to_time_t(timeStamp);
    const auto err = localtime_s(&time, &t);

    // https://stackoverflow.com/questions/28977585/how-to-get-put-time-into-a-variable
    auto timeString = std::string(MAX_PATH, 0);
    const auto sz = std::strftime(timeString.data(), timeString.size(), pFmt, &time);
    timeString.resize(sz);

    return timeString;
}
