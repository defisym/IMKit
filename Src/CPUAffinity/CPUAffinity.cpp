#include "CPUAffinity.h"

#include "macro.h"

DWORD CPUAffinity::SetThreadAffinity(const HANDLE hThread, const DWORD_PTR dwThreadAffinityMask) {
#ifdef PROCESS_SET_AFFINITY
    // Do not bind process -> may using multi-thread later
    //                        unwrap, etc. uses auto parallel
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-setprocessaffinitymask
    if (!SetThreadAffinityMask(hThread, dwThreadAffinityMask)) {
        return GetLastError();
    }

    return ERROR_SUCCESS;
#endif
}

DWORD_PTR CPUAffinity::GetCPUMask(const size_t CPUID) {
    return static_cast<DWORD_PTR>(0x01) << CPUID;
}
