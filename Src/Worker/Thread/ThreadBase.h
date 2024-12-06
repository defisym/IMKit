#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

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
    HANDLE threadHandle = nullptr;
    int threadReturn = 0;

    SDL_atomic_t detachThread = { ThreadConstant::ATTACH };
    SDL_atomic_t quitThread = { ThreadConstant::WORK };

    // generate a name if name is nullptr
    static const char* GetThreadName(const char* pName = nullptr);

public:
    virtual ~ThreadBase() = default;
    virtual int Worker() = 0;

    // ReSharper clang tidy is disabled here
    // as mark base virtual or mark derive override
    // will cause compile error, it's by design
    // to hide this member

    // called before thread start, for re-alloc
    virtual void StartCallback();
    // ReSharper disable once CppHiddenFunction
    bool Start(const ThreadInfo& info = {});
    // ReSharper disable once CppHiddenFunction
    bool ReStart(const ThreadInfo& info = {});

    // called when thread actually start
    virtual void ExecuteCallback();

    // called after thread stop
    virtual void StopCallback();
    virtual bool Stop();

    [[nodiscard]] SDL_threadID GetThreadID() const;
    [[nodiscard]] HANDLE GetThreadHandle() const;
};

namespace MutexConstant {
    constexpr int WAKE = 0;
    constexpr int HIBERNATE = 1;

    constexpr int LOOP = 0;
    constexpr int BREAK = 1;
}

struct ThreadHibernateInfo : ThreadInfo {
    bool bHibernateAtStart = false;
};

class ThreadHibernate :public ThreadBase {
    SDL_mutex* pMutex = nullptr;
    SDL_cond* pCond = nullptr;

    SDL_atomic_t loopState = { MutexConstant::LOOP };
    SDL_atomic_t hibernateState = { MutexConstant::WAKE };

public:
    ThreadHibernate();
    ~ThreadHibernate() override;

    // ReSharper clang tidy is disabled here
    // as mark base virtual or mark derive override
    // will cause compile error, it's by design
    // to hide this member

    // ReSharper disable once CppHidingFunction
    bool Start(const ThreadInfo& info = {}) = delete;
    bool Start(const ThreadHibernateInfo& info = {});
    // ReSharper disable once CppHidingFunction
    bool ReStart(const ThreadInfo& info = {}) = delete;
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