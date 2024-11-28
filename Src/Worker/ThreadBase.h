#pragma once

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_atomic.h>

namespace ThreadConstanst {
    constexpr int WORK = 0;
    constexpr int WAITING = 1;
    constexpr int QUIT = 2;

    constexpr int ATTACH = 0;
    constexpr int DETACH = 1;
}

struct ThreadInfo {
    const char* pName = nullptr;
    bool bDetach = false;
};

class ThreadBase {
    SDL_Thread* pThread = nullptr;
    SDL_threadID threadId = 0;
    int threadReturn = 0;

    SDL_atomic_t detachThread = { ThreadConstanst::ATTACH };
    SDL_atomic_t quitThread = { ThreadConstanst::WORK };

    // generate a name if name is nullptr
    static const char* GetThreadName(const char* pName = nullptr);

public:
    virtual ~ThreadBase() = default;
    virtual int Worker() = 0;

    bool Start(const ThreadInfo& info = {});
    bool ReStart(const ThreadInfo& info = {});
    bool Stop();

    SDL_threadID GetThreadID() const { return threadId; }
};