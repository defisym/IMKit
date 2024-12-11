#pragma once

#include <functional>

#include "LockHelper.h"

#include "Thread/ThreadBase.h"

struct Ctx;

struct WorkerBase : ThreadHibernate {
    Ctx* pCtx = nullptr;
    Lock<SDL_SpinLock> UILock;

    auto GetUILockHelper() { /*RVO*/ return SpinLockHelper{ UILock }; }

    using CallbackType = std::function<void()>;
    CallbackType executeCallback;

    WorkerBase(Ctx* p, CallbackType cb);
    void ExecuteCallback() override;
};