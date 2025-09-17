#pragma once

#include <functional>

#include "LockHelper.h"

#include "macro.h"

#include "Thread/ThreadBase.h"
#ifdef MULTITHREAD_SLEEP_WHEN_NODATA
#include "Utilities/WaitTimer.h"
#endif

struct IMGUIContext;
struct WorkerBase : ThreadHibernate {
    IMGUIContext* pCtx = nullptr;
    Lock<SDL_SpinLock> UILock;

    auto GetUILockHelper() { /*RVO*/ return SpinLockHelper{ UILock }; }

    using CallbackType = std::function<void()>;
    CallbackType executeCallback;

    WorkerBase(IMGUIContext* p, CallbackType executeCb);
    void ExecuteCallback() override;

#ifdef MULTITHREAD_SLEEP_WHEN_NODATA
    WaitTimer timer = {};
#endif
};