#pragma once

#include <chrono>
#include <string>

std::string GetFormattedTimeStamp(const decltype(std::chrono::system_clock::now()) timeStamp = std::chrono::system_clock::now(),
    char const* pFmt = "%Y-%m-%d %H-%M-%S");