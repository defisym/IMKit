#pragma once

// This class used to utilize the multicore processor
//
// Hyper thread should be disabled in BIOS
// as we manually set the affinity of threads
//
// for OTDR, there should be three types of processes
//  1. UI: E-Core
//      update & render routine are called each frame
//      the main performance bottleneck is the display of huge chat
//      this part will be optimized if necessary, aka display uses more than 16ms
//  2. Reader: E-Core
//      due to the limitation of DAC upload speed
//      a reader thread should always run to retrieve data
//  3. Worker: P-Core
//      demodulate algorithm cannot be parallized
//      so it prefer higher performance, we should put it in P-Core

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <cstdint>

struct CPUAffinity {
    virtual ~CPUAffinity() = default;

    // wrapper for disable by macro
    static DWORD SetPriorityClass(HANDLE hProcess, DWORD dwPriorityClass);
    static DWORD SetThreadAffinity(HANDLE hThread, DWORD_PTR dwThreadAffinityMask);
    static DWORD_PTR GetCPUMask(size_t CPUID);

    virtual DWORD BindToPerformaceCore(HANDLE hThread, size_t CPUID) = 0;
    virtual DWORD BindToEfficiencyCore(HANDLE hThread, size_t CPUID) = 0;
        
    enum class CoreState :std::uint8_t {
        Keep = 0,   // reserve

        Free,       // mark as Free: can be allocated
        Exclusive,  // mark as Exclusive: heave long term worker
    };

    virtual void SetPerformaceCoreState(size_t CPUID, CoreState state) = 0;
    virtual void SetEfficiencyCoreState(size_t CPUID, CoreState state) = 0;

    virtual DWORD BindToFreePerformaceCore(HANDLE hThread, CoreState state = CoreState::Keep) = 0;
    virtual DWORD BindToFreeEfficiencyCore(HANDLE hThread, CoreState state = CoreState::Keep) = 0;
};