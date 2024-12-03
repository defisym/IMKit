#pragma once

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_atomic.h>

namespace ThreadConstant {
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

    SDL_atomic_t detachThread = { ThreadConstant::ATTACH };
    SDL_atomic_t quitThread = { ThreadConstant::WORK };

    // generate a name if name is nullptr
    static const char* GetThreadName(const char* pName = nullptr);

public:
    virtual ~ThreadBase() = default;
    virtual int Worker() = 0;

    // called before thread start, for re-alloc
    virtual void StartCallback();
    bool Start(const ThreadInfo& info = {});
    bool ReStart(const ThreadInfo& info = {});

    // called after thread stop
    virtual void StopCallback();
    virtual bool Stop();

    [[nodiscard]] SDL_threadID GetThreadID() const { return threadId; }
};

#include <_DeLib/LockHelper.h>

namespace MutexConstant {
    constexpr int WAKE = 0;
    constexpr int HIBERNATE = 1;

    constexpr int LOOP = 0;
    constexpr int BREAK = 1;
}

struct ThreadHibernateInfo :ThreadInfo {
    bool bHibernateAtStart = false;
};

class ThreadHibernate :ThreadBase {
    SDL_mutex* pMutex = nullptr;
    SDL_cond* pCond = nullptr;

    SDL_atomic_t loopState = { MutexConstant::LOOP };
    SDL_atomic_t hibernateState = { MutexConstant::WAKE };

public:
    ThreadHibernate();
    ~ThreadHibernate() override;

    bool Start(const ThreadHibernateInfo& info = {});
    bool ReStart(const ThreadHibernateInfo& info = {});

    bool Stop() override;

    // called before thread hibernate
    virtual void HibernateCallback();
    void Hibernate();

    // called after thread wake, for re-alloc
    virtual void WakeCallback();
    void Wake();

    int Worker() override;
    virtual int LoopBody() = 0;
    void BreakLoop();
};