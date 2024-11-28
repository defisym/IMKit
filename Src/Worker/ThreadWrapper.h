#pragma once

#include <memory>
#include <type_traits>

#include "ThreadBase.h"

template<typename ThreadType>
    requires std::is_base_of_v<ThreadBase, ThreadType>
class ThreadWrapper {
    std::unique_ptr<ThreadBase> thread;

public:
    template <class... ParamTypes>
    ThreadWrapper(ParamTypes&&... args) {
        thread = std::make_unique<ThreadType>(std::forward<ParamTypes>(args)...);
    }
    ~ThreadWrapper() { [[maybe_unused]] const auto b = Stop(); }

    bool Start(const ThreadInfo& info = {}) const { return thread->Start(info); }
    bool ReStart(const ThreadInfo& info = {}) const { return thread->ReStart(info); }
    bool Stop() const { return thread->Stop(); }
};
