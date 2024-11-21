#include "13700K.h"

Intel13700K::Intel13700K() {
    size_t CPUID = 0;

    for (; CPUID < 8; CPUID++) {
        PCoreMask.emplace_back(GetCPUMask(CPUID));
    }

    PCoreState.resize(PCoreMask.size());
    for (auto& it : PCoreState) { it = CoreState::Free; }

    for (; CPUID < 16; CPUID++) {
        ECoreMask.emplace_back(GetCPUMask(CPUID));
    }

    ECoreState.resize(ECoreMask.size());
    for (auto& it : ECoreState) { it = CoreState::Free; }
}

DWORD Intel13700K::BindToPerformaceCore(const HANDLE hThread, const size_t CPUID) {
    if (CPUID >= PCoreMask.size()) { return ERROR_INVALID_PARAMETER; }

    return SetThreadAffinity(hThread, PCoreMask[CPUID]);
}
DWORD Intel13700K::BindToEfficiencyCore(const HANDLE hThread, const size_t CPUID) {
    if (CPUID >= ECoreMask.size()) { return ERROR_INVALID_PARAMETER; }

    return SetThreadAffinity(hThread, ECoreMask[CPUID]);
}

void Intel13700K::SetPerformaceCoreState(const size_t CPUID, const CoreState state) {
    if (CPUID >= PCoreState.size()) { return; }

    PCoreState[CPUID] = state;
}
void Intel13700K::SetEfficiencyCoreState(const size_t CPUID, const CoreState state) {
    if (CPUID >= ECoreState.size()) { return; }

    ECoreState[CPUID] = state;
}

DWORD Intel13700K::BindToFreePerformaceCore(const HANDLE hThread, const CoreState state) {
    const auto it = std::ranges::find(PCoreState, CoreState::Free);
    if (it == PCoreState.end()) { return ERROR_SUCCESS; }

    const auto CPUID = std::distance(PCoreState.begin(), it);

    if (state != CoreState::Keep) { SetPerformaceCoreState(CPUID, state); }
    return BindToPerformaceCore(hThread, CPUID);
}
DWORD Intel13700K::BindToFreeEfficiencyCore(const HANDLE hThread, const CoreState state) {
    const auto it = std::ranges::find(ECoreState, CoreState::Free);
    if (it == ECoreState.end()) { return ERROR_SUCCESS; }

    const auto CPUID = std::distance(ECoreState.begin(), it);

    if (state != CoreState::Keep) { SetEfficiencyCoreState(CPUID, state); }
    return BindToEfficiencyCore(hThread, CPUID);
}
