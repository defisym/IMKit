#include "TimeStampHelper.h"

std::string GetFormattedTimeStamp(const decltype(std::chrono::system_clock::now()) timeStamp,
    char const* pFmt) {
    std::tm time = {};
    const auto t = std::chrono::system_clock::to_time_t(timeStamp);
    [[maybe_unused]] const auto err = localtime_s(&time, &t);

    // https://stackoverflow.com/questions/28977585/how-to-get-put-time-into-a-variable
    auto timeString = std::string(260, 0);
    const auto sz = std::strftime(timeString.data(), timeString.size(), pFmt, &time);
    timeString.resize(sz);

    return timeString;
}
