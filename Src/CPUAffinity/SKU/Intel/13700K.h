#pragma once

#include "CPUAffinity/CPUAffinity.h"

#include <vector>

// 13700K: 8P 8E
struct Intel13700K :CPUAffinity {
    std::vector<DWORD_PTR> PCoreMask = {};
    std::vector<DWORD_PTR> ECoreMask = {};

    Intel13700K();

    DWORD BindToPerformaceCore(HANDLE hThread, size_t CPUID) override;
    DWORD BindToEfficiencyCore(HANDLE hThread, size_t CPUID) override;

    std::vector<CoreState> PCoreState = {};
    std::vector<CoreState> ECoreState = {};

    virtual void SetPerformaceCoreState(size_t CPUID, CoreState state) override;
    virtual void SetEfficiencyCoreState(size_t CPUID, CoreState state) override;

    virtual DWORD BindToFreePerformaceCore(HANDLE hThread, CoreState state = CoreState::Keep) override;
    virtual DWORD BindToFreeEfficiencyCore(HANDLE hThread, CoreState state = CoreState::Keep) override;
};