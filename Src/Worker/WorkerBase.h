#pragma once

#include <functional>

#include "LockHelper.h"

#include "macro.h"

#include "Thread/ThreadBase.h"
#ifdef MULTITHREAD_SLEEP_WHEN_NODATA
#include "Utilities/WaitTimer.h"
#endif

struct Ctx;

struct WorkerBase : ThreadHibernate {
    Ctx* pCtx = nullptr;
    Lock<SDL_SpinLock> UILock;

    auto GetUILockHelper() { /*RVO*/ return SpinLockHelper{ UILock }; }

    using CallbackType = std::function<void()>;
    CallbackType executeCallback;

    WorkerBase(Ctx* p, CallbackType executeCb);
    void ExecuteCallback() override;

#ifdef MULTITHREAD_SLEEP_WHEN_NODATA
    WaitTimer timer = {};
#endif
};