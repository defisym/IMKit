#include "TimeStampHelper.h"

// Windows Style:
//    LPSYSTEMTIME lpSystemTime = new SYSTEMTIME;
//    GetLocalTime(lpSystemTime);
//
//    const auto time = std::format(L"{:0>4d}-{:0>2d}-{:0>2d}_{:0>2d}-{:0>2d}-{:0>2d}.csv",
//        lpSystemTime->wYear,
//        lpSystemTime->wMonth,
//        lpSystemTime->wDay,
//        lpSystemTime->wHour,
//        lpSystemTime->wMinute,
//        lpSystemTime->wSecond);

std::string GetFormattedTimeStamp(const TimeStampHelper::TimeStamp timeStamp,
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
