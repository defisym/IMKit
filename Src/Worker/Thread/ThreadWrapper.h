#pragma once

#include <memory>
#include <type_traits>

#include "ThreadBase.h"

template<typename ThreadType>
concept ThreadTypeContept = requires(ThreadType thread) {
    std::is_base_of_v<ThreadBase, ThreadType>;
};

template<ThreadTypeContept ThreadType>
class ThreadWrapper {
    using Thread = std::unique_ptr<ThreadType>;
    Thread thread;

public:
    template <class... ParamTypes>
    ThreadWrapper(ParamTypes&&... args) {
        thread = std::make_unique<ThreadType>(std::forward<ParamTypes>(args)...);
    }
    ~ThreadWrapper() { [[maybe_unused]] const auto b = GetThread()->Stop(); }

    const Thread& GetThread() { return thread; }    
};
