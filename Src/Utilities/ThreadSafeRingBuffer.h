#pragma once

#include <SDL_atomic.h>
#pragma comment(lib, "SDL2.lib")

#include "RingBuffer.h"
#include "LockHelper.h"

template<typename Type = OTDRProcessValueType>
struct ThreadSafeRingBuffer :private RingBuffer<Type> {
    SDL_SpinLock lock = 0;

    ThreadSafeRingBuffer() = default;
    ThreadSafeRingBuffer(const size_t sz) :RingBuffer<Type>(sz) {}

    ~ThreadSafeRingBuffer() override {
        const auto lockHelper = SpinLockHelper(&lock);
        RingBuffer<Type>::Release();
    }

    void WriteData(const Type* pBuf, const size_t sz) override {
        const auto lockHelper = SpinLockHelper(&lock);
        RingBuffer<Type>::WriteData(pBuf, sz);
    }
    void ReadData(Type* pBuf, const size_t sz) override {
        const auto lockHelper = SpinLockHelper(&lock);
        RingBuffer<Type>::ReadData(pBuf, sz);
    }
    void ResetIndex() override {
        const auto lockHelper = SpinLockHelper(&lock);
        RingBuffer<Type>::ResetIndex();
    }
    void ResetBuffer() {
        const auto lockHelper = SpinLockHelper(&lock);
        RingBuffer<Type>::Reset();
    }
};