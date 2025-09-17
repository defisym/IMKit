#pragma once

#include "ThreadBase.h"
#include "ThreadWrapper.h"

struct ThreadHandlerBase {
    template<ThreadTypeConcept ThreadType>
    using Thread = std::unique_ptr<ThreadWrapper<ThreadType>>;

	template <ThreadTypeConcept ThreadType>
    static HANDLE GetThreadHandle(Thread<ThreadType>& thread) {
        return thread->GetThread()->GetThreadHandle();
    }

    // init the thread and hibernate it by default
    template <ThreadTypeConcept ThreadType, class... ParamTypes>
    void InitThread(Thread<ThreadType>& thread,
        const char* pName, ParamTypes&&... args) {
        thread = std::make_unique<ThreadWrapper<ThreadType>>(std::forward<ParamTypes>(args)...);
        const ThreadInfo info = { .pName = pName,.bDetach = false };
        const ThreadHibernateInfo infoHibernate = { info, true };
        thread->GetThread()->Start(infoHibernate);
    }

	template <ThreadTypeConcept ThreadType>
    static auto GetUILockHelper(Thread<ThreadType>& thread) {
        return thread->GetThread()->GetUILockHelper();
    }
};