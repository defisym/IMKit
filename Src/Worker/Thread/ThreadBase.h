#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <memory>

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
    static const char* GenerateThreadName(const char* pName = nullptr);

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

    // called before thread stop
    virtual void BeforeStopCallback();
    // called after thread stop
    virtual void AfterStopCallback();

    bool WaitStop();
    bool Stop();

    [[nodiscard]] SDL_threadID GetThreadID() const;
    [[nodiscard]] HANDLE GetThreadHandle() const;

    [[nodiscard]] const char* GetThreadName() const;
};

#include <HibernateContext.h>

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
    std::shared_ptr<HibernateContext> pContext = nullptr;

    SDL_atomic_t loopState = { MutexConstant::LOOP };
    SDL_atomic_t hibernateState = { MutexConstant::WAKE };

public:
    ThreadHibernate();
    explicit ThreadHibernate(const std::shared_ptr<HibernateContext>& ctx);
    ~ThreadHibernate() override = default;

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

    void BeforeStopCallback() override;

    // called before thread hibernate
    virtual void HibernateCallback();
    virtual void Hibernate();
    virtual void WaitUntilHibernated();

    // called after thread wake, for re-alloc
    virtual void WakeCallback();
    virtual void Wake();

    int Worker() override;
    virtual int LoopBody() = 0;
    void BreakLoop();
};