#pragma once

#define NOMINMAX
#include <Windows.h>
#include <cstdint>

struct WaitTimer {
    bool bInit = false;
    HANDLE hTimer = nullptr;

	LARGE_INTEGER performanceCount = {};
	LARGE_INTEGER performanceFrequency = {};

    WaitTimer(const char* pName = nullptr) {
        hTimer = CreateWaitableTimerExA(nullptr,
            pName,
            CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
            TIMER_ALL_ACCESS);
        if (!hTimer) { return; };

        if (!QueryPerformanceCounter(&performanceCount)) { return; }
        if (!QueryPerformanceFrequency(&performanceFrequency)) { return; }

        bInit = true;
    }
    ~WaitTimer() {
        if(hTimer){ CloseHandle(hTimer); }
    }

    DWORD WaitMicroSecond(std::int64_t microsecond) const {
        LARGE_INTEGER dueTime = {};
        // in 100 nanosecond intervals, negative values indicate relative time
        dueTime.QuadPart = -static_cast<LONGLONG>(microsecond * 10); 

        auto bRet = SetWaitableTimer(hTimer, &dueTime, 0, nullptr, nullptr, FALSE);
        if (!bRet) { return GetLastError(); }

        auto dRet = WaitForSingleObject(hTimer, INFINITE);
        if (dRet == WAIT_FAILED) { return GetLastError(); }

        return 0;
    }

    DWORD WaitMilliSecond(std::int64_t millisecond) const {
        return WaitMicroSecond(millisecond * 1000);
    }
    DWORD WaitMilliSecond(double millisecond) const {
        return WaitMicroSecond(static_cast<std::int64_t>(millisecond * 1000));
    }
};