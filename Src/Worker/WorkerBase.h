#pragma once

#include <functional>

#include "Thread/ThreadBase.h"

struct Ctx;

struct WorkerBase : ThreadHibernate {
    using ExecuteCallbackType = std::function<void()>;

    Ctx* pCtx = nullptr;
    ExecuteCallbackType executeCallback;

    WorkerBase(Ctx* p, ExecuteCallbackType cb);
    void ExecuteCallback() override;
};