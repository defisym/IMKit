#pragma once

#include <format>

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_atomic.h>

namespace ThreadState {
    constexpr int Work = 0;
    constexpr int Quit = 1;
}

class ThreadBase {
    SDL_atomic_t quit = { ThreadState::Work };
    SDL_Thread* pThread = nullptr;
    SDL_threadID threadId = 0;
    int threadReturn = 0;

public:
    virtual ~ThreadBase() { Stop(); }
    virtual int Worker() = 0;

    void Start(const char* pName = nullptr) {
        // generate a name if pName is nullptr
        static size_t threadIndex = 0u;
        static std::string threadName = {};

        threadIndex++;
        if (pName == nullptr) {
            threadName = std::format("thread_{}", threadIndex);
            pName = threadName.c_str();
        }

        pThread = SDL_CreateThread([] (void* pData)->int {
            return static_cast<ThreadBase*>(pData)->Worker();
        },
        pName, this);
    }
    void Stop() {
        // already triggered
        if (SDL_AtomicGet(&quit) == ThreadState::Quit) { return; }

        SDL_AtomicSet(&quit, ThreadState::Quit);
        SDL_WaitThread(pThread, &threadReturn);
    }
};