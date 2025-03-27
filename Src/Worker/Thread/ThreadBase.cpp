#include "ThreadBase.h"

#include <format>

const char* ThreadBase::GenerateThreadName(const char* pName) {
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
        GenerateThreadName(info.pName), this);

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

void ThreadBase::ExecuteCallback() {
    threadId = SDL_GetThreadID(pThread);

    // permission that can bind CPU core
    threadHandle = OpenThread(THREAD_SET_INFORMATION | THREAD_QUERY_INFORMATION,
        TRUE, threadId);
    SDL_AtomicSet(&quitThread, ThreadConstant::WORK);
}

void ThreadBase::BeforeStopCallback() {}

void ThreadBase::AfterStopCallback() {}

bool ThreadBase::WaitStop() {
    return SDL_AtomicGet(&quitThread) == ThreadConstant::WAITING;
}

bool ThreadBase::Stop() {
    do {
        if (pThread == nullptr) { break; }
        if (SDL_AtomicGet(&detachThread) == ThreadConstant::DETACH) { break; }
        if (SDL_AtomicGet(&quitThread) == ThreadConstant::QUIT) { break; }

        SDL_AtomicSet(&quitThread, ThreadConstant::WAITING);
        BeforeStopCallback();
        SDL_WaitThread(pThread, &threadReturn);
        AfterStopCallback();

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

const char* ThreadBase::GetThreadName() const {
    return SDL_GetThreadName(pThread);
}

#include <_DeLib/LockHelper.h>

ThreadHibernate::ThreadHibernate() {
    pContext = std::make_shared<HibernateContext>();
}

ThreadHibernate::ThreadHibernate(const std::shared_ptr<HibernateContext>& ctx)
    :pContext(ctx) {
}

bool ThreadHibernate::Start(const ThreadHibernateInfo& info) {
    if (info.bHibernateAtStart) { Hibernate(); }
    return ThreadBase::Start(info);
}

bool ThreadHibernate::ReStart(const ThreadHibernateInfo& info) {
    Stop(); return Start(info);
}

void ThreadHibernate::BeforeStopCallback() {
    BreakLoop(); Wake();
}

void ThreadHibernate::HibernateCallback() {}

void ThreadHibernate::Hibernate() {
    if (SDL_AtomicGet(&hibernateState) == MutexConstant::HIBERNATE) { return; }
    SDL_AtomicSet(&hibernateState, MutexConstant::HIBERNATE);
}

void ThreadHibernate::WaitUntilHibernated() {
    Hibernate();
    // get mutex -> hibernated
    const auto helper = pContext->GetLockHelper();
}

void ThreadHibernate::WakeCallback() {}

void ThreadHibernate::Wake() {
    if (SDL_AtomicGet(&hibernateState) != MutexConstant::HIBERNATE) { return; }
    pContext->Wake();
}

void ThreadHibernate::BreakLoop() {
    if (SDL_AtomicGet(&loopState) == MutexConstant::BREAK) { return; }
    SDL_AtomicSet(&loopState, MutexConstant::BREAK);
}

int ThreadHibernate::Worker() {
    auto lockerHelper = pContext->GetLockHelper();

    while (true) {
        if (SDL_AtomicGet(&hibernateState) == MutexConstant::HIBERNATE) {
            HibernateCallback();
#ifdef _DEBUG
            OutputDebugStringA(std::format("Thread {}: Hibernate\n", GetThreadName()).c_str());
#endif
            pContext->Hibernate();
#ifdef _DEBUG
            OutputDebugStringA(std::format("Thread {}: Wake\n", GetThreadName()).c_str());
#endif
            SDL_AtomicSet(&hibernateState, MutexConstant::WAKE);
            WakeCallback();
        }

        if (SDL_AtomicGet(&loopState) == MutexConstant::BREAK) {
            break;
        }

//#ifdef _DEBUG
//        OutputDebugStringA(std::format("Thread {}: Call LoopBody\n", GetThreadName()).c_str());
//#endif

        LoopBody();
    }

    return 0;
}
