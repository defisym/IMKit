#include "WorkerBase.h"

WorkerBase::WorkerBase(Ctx* p, ExecuteCallbackType cb)
    :pCtx(p), executeCallback(std::move(cb)) {}

void WorkerBase::ExecuteCallback() {
    // get info first
    ThreadHibernate::ExecuteCallback();
    // update by the info
    executeCallback();
}
