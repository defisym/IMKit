#pragma once

#include <vector>

#include "../Src/DLL/include/OTDR.h"

#include "ThreadBase.h"

// usually sample rate is above 2000Hz, aka less than 0.5ms
// mutex & cond will let operate system handle the thread
// which will take longer, usuall several ms
// that's why spin lock is used here
struct DataQueue {
    SDL_SpinLock lock = 0;

    struct Data {
        size_t sz = 0;
        OTDRProcessValueType* pPhase = nullptr;
        OTDRProcessValueType* pAmplitude = nullptr;

        Data() {
            //Context_UpdateToBuffer();
        }
        ~Data() {
            delete[] pAmplitude;
            delete[] pPhase;
        }
    };

    std::vector<Data> queue;

    DataQueue() { }
    ~DataQueue() { }
};

struct Ctx;

struct DataReader :ThreadHibernate {
    Ctx* pCtx = nullptr;

    DataReader(Ctx* p);
    int LoopBody() override;
};