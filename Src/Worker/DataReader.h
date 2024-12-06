#pragma once

#include "../Src/DLL/include/OTDR.h"

#include "Thread/ThreadBase.h"
#include "Utilities/ThreadSafeRingBuffer.h"

// usually sample rate is above 2000Hz, aka less than 0.5ms
// mutex & cond will let operate system handle the thread
// which will take longer, usuall several ms
// that's why spin lock is used here
struct DataQueue {
    // ------------------------
    // Following are protected by lock
    // ------------------------
    ThreadSafeRingBuffer<> phaseBuffer;
    ThreadSafeRingBuffer<> amplitudeBuffer;
    // ------------------------
    // Above are protected by lock
    // ------------------------

    size_t frameSize = 0;

    void UpdateQueue(const size_t count, const size_t size);

    void AddData(const OTDRContextHandle handle);

    OTDRProcessValueType* GetPhaseData();

    OTDRProcessValueType* GetAmplitudeData();
};

struct Ctx;

struct DataReader :ThreadHibernate {
    Ctx* pCtx = nullptr;
    DataQueue queue;

    DataReader(Ctx* p);

    void WakeCallback() override;
    int LoopBody() override;
};