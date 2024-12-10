#pragma once

#include <functional>

#include "LockHelper.h"

#include "Thread/ThreadBase.h"

struct Ctx;

struct WorkerBase : ThreadHibernate {
    Ctx* pCtx = nullptr;
    Lock<SDL_SpinLock> UILock;

    auto GetLockHelper() { /*RVO*/ return SpinLockHelper{ UILock }; }

    using ExecuteCallbackType = std::function<void()>;
    ExecuteCallbackType executeCallback;

    WorkerBase(Ctx* p, ExecuteCallbackType cb);
    void ExecuteCallback() override;
};