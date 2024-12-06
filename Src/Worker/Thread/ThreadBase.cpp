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

void ThreadBase::StartCallback() {}

bool ThreadBase::Start(const ThreadInfo& info) {
    if (pThread != nullptr) { return false; }

    StartCallback();
    pThread = SDL_CreateThread([] (void* pData)->int {
            const auto p = static_cast<ThreadBase*>(pData);
            p->ExecuteCallback();

            return p->Worker();
        },
        GetThreadName(info.pName), this);

    if (pThread == nullptr) {
        [[maybe_unused]] const auto err = SDL_GetError();
        return false;
    }

    if (info.bDetach) {
        SDL_AtomicSet(&detachThread, ThreadConstant::DETACH);
        SDL_DetachThread(pThread);
    }

    return true;
}

bool ThreadBase::ReStart(const ThreadInfo& info) {
    Stop(); return Start(info);
}

inline void ThreadBase::ExecuteCallback() {
    threadId = SDL_GetThreadID(pThread);

    // permission that can bind CPU core
    threadHandle = OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION,
        TRUE, threadId);
    SDL_AtomicSet(&quitThread, ThreadConstant::WORK);
}

void ThreadBase::StopCallback() {}

bool ThreadBase::Stop() {
    do {
        if (pThread == nullptr) { break; }
        if (SDL_AtomicGet(&detachThread) == ThreadConstant::DETACH) { break; }
        if (SDL_AtomicGet(&quitThread) == ThreadConstant::QUIT) { break; }

        SDL_AtomicSet(&quitThread, ThreadConstant::WAITING);
        SDL_WaitThread(pThread, &threadReturn);
        StopCallback();

        CloseHandle(threadHandle);
        threadHandle = nullptr;
        threadId = 0;
        pThread = nullptr;

        SDL_AtomicSet(&quitThread, ThreadConstant::QUIT);

        return true;
    } while (false);

    return false;
}

SDL_threadID ThreadBase::GetThreadID() const { return threadId; }

HANDLE ThreadBase::GetThreadHandle() const { return threadHandle; }

#include <_DeLib/LockHelper.h>

ThreadHibernate::ThreadHibernate() {
    pMutex = SDL_CreateMutex();
    pCond = SDL_CreateCond();
}

ThreadHibernate::~ThreadHibernate() {
    SDL_DestroyCond(pCond);
    SDL_DestroyMutex(pMutex);
}

bool ThreadHibernate::Start(const ThreadHibernateInfo& info) {
    if (info.bHibernateAtStart) { Hibernate(); }
    return ThreadBase::Start(info);
}

bool ThreadHibernate::ReStart(const ThreadHibernateInfo& info) {
    Stop(); return Start(info);
}

bool ThreadHibernate::Stop() {
    Wake(); BreakLoop(); return ThreadBase::Stop();
}

void ThreadHibernate::HibernateCallback() {}

void ThreadHibernate::Hibernate() {
    if (SDL_AtomicGet(&hibernateState) == MutexConstant::HIBERNATE) { return; }
    SDL_AtomicSet(&hibernateState, MutexConstant::HIBERNATE);
}

void ThreadHibernate::WakeCallback() {}

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
            HibernateCallback();
            SDL_CondWait(pCond, pMutex);
            SDL_AtomicSet(&hibernateState, MutexConstant::WAKE);
            WakeCallback();
        }

        LoopBody();
    }

    return 0;
}
