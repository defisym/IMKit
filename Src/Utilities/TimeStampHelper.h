#pragma once

#include <chrono>
#include <string>

namespace TimeStampHelper {
    using TimeStamp = decltype(std::chrono::system_clock::now());
}

std::string GetFormattedTimeStamp(const TimeStampHelper::TimeStamp timeStamp = std::chrono::system_clock::now(),
    char const* pFmt = "%Y-%m-%d %H-%M-%S");
