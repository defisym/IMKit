#include "ThreadBase.h"

#include <format>

const char* ThreadBase::GetThreadName(const char* pName) {
    static size_t threadIndex = 0u;
    static std::string threadName = {};

    if (pName != nullptr) { return pName; }

    threadName = std::format("thread_{}", threadIndex);
    pName = threadName.c_str();
    threadIndex++;

    return pName;
}

bool ThreadBase::Start(const ThreadInfo& info) {
    if (pThread != nullptr) { return false; }

    pThread = SDL_CreateThread([] (void* pData)->int {
            return static_cast<ThreadBase*>(pData)->Worker();
        },
        GetThreadName(info.pName), this);

    if (pThread == nullptr) {
        [[maybe_unused]] const auto err = SDL_GetError();
        return false;
    }

    threadId = SDL_GetThreadID(pThread);

    if (info.bDetach) {
        SDL_AtomicSet(&detachThread, ThreadConstanst::DETACH);
        SDL_DetachThread(pThread);
    }

    return true;
}

bool ThreadBase::ReStart(const ThreadInfo& info) {
    Stop(); pThread = nullptr;
    SDL_AtomicSet(&quitThread, ThreadConstanst::WORK);

    return Start(info);
}

bool ThreadBase::Stop() {
    do {
        if (pThread == nullptr) { break; }
        if (SDL_AtomicGet(&detachThread) == ThreadConstanst::DETACH) { break; }
        if (SDL_AtomicGet(&quitThread) == ThreadConstanst::QUIT) { break; }

        SDL_AtomicSet(&quitThread, ThreadConstanst::WAITING);
        SDL_WaitThread(pThread, &threadReturn);
        SDL_AtomicSet(&quitThread, ThreadConstanst::QUIT);

        return true;
    } while (false);

    return false;
}
