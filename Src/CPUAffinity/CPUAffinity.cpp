#include "CPUAffinity.h"

#include "macro.h"

CPUAffinity::CPUAffinity(const CPUInfo& info) {
    size_t CPUID = 0;

    const auto pCore = info.bHyperThread ? 2 * info.pCore : info.pCore;
    const auto pCoreStep = info.bHyperThread ? 2 : 1;
    for (; CPUID < pCore; CPUID += pCoreStep) {
        PCoreMask.emplace_back(GetCPUMask(CPUID));
    }

    PCoreState.resize(PCoreMask.size());
    for (auto& it : PCoreState) { it = CoreState::Free; }

    const auto totalCore = pCore + info.eCore;
    for (; CPUID < totalCore; CPUID++) {
        ECoreMask.emplace_back(GetCPUMask(CPUID));
    }

    ECoreState.resize(ECoreMask.size());
    for (auto& it : ECoreState) { it = CoreState::Free; }
}

DWORD CPUAffinity::SetPriorityClass(HANDLE hProcess, DWORD dwPriorityClass) {
#ifdef PROCESS_SET_PRIORITY
    // Set priority
    // https://learn.microsoft.com/zh-cn/windows/win32/api/processthreadsapi/nf-processthreadsapi-setpriorityclass
    if (!::SetPriorityClass(hProcess, dwPriorityClass)) {
        return GetLastError();
    }
#endif

    return ERROR_SUCCESS;
}

DWORD CPUAffinity::SetThreadAffinity(const HANDLE hThread, const DWORD_PTR dwThreadAffinityMask) {
#ifdef PROCESS_SET_AFFINITY
    // Do not bind process -> may using multi-thread later
    //                        unwrap, etc. uses auto parallel
    // https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-setprocessaffinitymask
    if (!::SetThreadAffinityMask(hThread, dwThreadAffinityMask)) {
        return GetLastError();
    }
#endif

    return ERROR_SUCCESS;
}

DWORD_PTR CPUAffinity::GetCPUMask(const size_t CPUID) {
    return static_cast<DWORD_PTR>(0x01) << CPUID;
}

DWORD CPUAffinity::BindToPerformanceCore(const HANDLE hThread, const size_t CPUID) const {
    if (CPUID >= PCoreMask.size()) { return ERROR_INVALID_PARAMETER; }

    return SetThreadAffinity(hThread, PCoreMask[CPUID]);
}
DWORD CPUAffinity::BindToEfficiencyCore(const HANDLE hThread, const size_t CPUID) const {
    if (CPUID >= ECoreMask.size()) { return ERROR_INVALID_PARAMETER; }

    return SetThreadAffinity(hThread, ECoreMask[CPUID]);
}

void CPUAffinity::SetPerformanceCoreState(const size_t CPUID, const CoreState state) {
    if (CPUID >= PCoreState.size()) { return; }

    PCoreState[CPUID] = state;
}
void CPUAffinity::SetEfficiencyCoreState(const size_t CPUID, const CoreState state) {
    if (CPUID >= ECoreState.size()) { return; }

    ECoreState[CPUID] = state;
}

DWORD CPUAffinity::BindToFreePerformanceCore(const HANDLE hThread, const CoreState state) {
    const auto it = std::ranges::find(PCoreState, CoreState::Free);
    if (it == PCoreState.end()) { return ERROR_SUCCESS; }

    const auto CPUID = std::distance(PCoreState.begin(), it);

    if (state != CoreState::Keep) { SetPerformanceCoreState(CPUID, state); }
    return BindToPerformanceCore(hThread, CPUID);
}
DWORD CPUAffinity::BindToFreeEfficiencyCore(const HANDLE hThread, const CoreState state) {
    const auto it = std::ranges::find(ECoreState, CoreState::Free);
    if (it == ECoreState.end()) { return ERROR_SUCCESS; }

    const auto CPUID = std::distance(ECoreState.begin(), it);

    if (state != CoreState::Keep) { SetEfficiencyCoreState(CPUID, state); }
    return BindToEfficiencyCore(hThread, CPUID);
}
