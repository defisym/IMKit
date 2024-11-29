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
        SDL_AtomicSet(&detachThread, ThreadConstant::DETACH);
        SDL_DetachThread(pThread);
    }

    return true;
}

bool ThreadBase::ReStart(const ThreadInfo& info) {
    Stop(); pThread = nullptr;
    SDL_AtomicSet(&quitThread, ThreadConstant::WORK);

    return Start(info);
}

bool ThreadBase::Stop() {
    do {
        if (pThread == nullptr) { break; }
        if (SDL_AtomicGet(&detachThread) == ThreadConstant::DETACH) { break; }
        if (SDL_AtomicGet(&quitThread) == ThreadConstant::QUIT) { break; }

        SDL_AtomicSet(&quitThread, ThreadConstant::WAITING);
        SDL_WaitThread(pThread, &threadReturn);
        SDL_AtomicSet(&quitThread, ThreadConstant::QUIT);

        return true;
    } while (false);

    return false;
}

ThreadHibernate::ThreadHibernate() {
    pMutex = SDL_CreateMutex();
    pCond = SDL_CreateCond();
}

ThreadHibernate::~ThreadHibernate() {
    SDL_DestroyCond(pCond);
    SDL_DestroyMutex(pMutex);
}

bool ThreadHibernate::Stop() {
    Wake();
    return ThreadBase::Stop();
}

void ThreadHibernate::Hibernate() {
    if (SDL_AtomicGet(&hibernateState) == MutexConstant::HIBERNATE) { return; }
    SDL_AtomicSet(&hibernateState, MutexConstant::HIBERNATE);
}

void ThreadHibernate::Wake() {
    if (SDL_AtomicGet(&hibernateState) != MutexConstant::HIBERNATE) { return; }

    auto lockerHelper = MutexLockHelper{ pMutex };
    SDL_CondSignal(pCond);
}

void ThreadHibernate::BreakLoop() {
    if (SDL_AtomicGet(&loopState) == MutexConstant::BREAK) { return; }
    SDL_AtomicSet(&loopState, MutexConstant::BREAK);
}

int ThreadHibernate::Worker() {
    auto lockerHelper = MutexLockHelper{ pMutex };

    while (true) {
        if (SDL_AtomicGet(&loopState) == MutexConstant::BREAK) {
            break;
        }

        if (SDL_AtomicGet(&hibernateState) == MutexConstant::HIBERNATE) {
            SDL_CondWait(pCond, pMutex);
            SDL_AtomicSet(&hibernateState, MutexConstant::WAKE);
        }

        LoopBody();
    }

    return 0;
}
