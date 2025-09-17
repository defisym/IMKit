#pragma once

#include "ThreadBase.h"
#include "ThreadWrapper.h"

struct ThreadHandlerBase {
    template<ThreadTypeContept ThreadType>
    using Thread = std::unique_ptr<ThreadWrapper<ThreadType>>;

	template <ThreadTypeContept ThreadType>
    static HANDLE GetThreadHandle(Thread<ThreadType>& thread) {
        return thread->GetThread()->GetThreadHandle();
    }

    // init the thread and hibernate it by default
    template <ThreadTypeContept ThreadType, class... ParamTypes>
    void InitThread(Thread<ThreadType>& thread,
        const char* pName, ParamTypes&&... args) {
        thread = std::make_unique<ThreadWrapper<ThreadType>>(std::forward<ParamTypes>(args)...);
        const ThreadInfo info = { .pName = pName,.bDetach = false };
        const ThreadHibernateInfo infoHibernate = { info, true };
        thread->GetThread()->Start(infoHibernate);
    }

	template <ThreadTypeContept ThreadType>
    static auto GetUILockHelper(Thread<ThreadType>& thread) {
        return thread->GetThread()->GetUILockHelper();
    }
};