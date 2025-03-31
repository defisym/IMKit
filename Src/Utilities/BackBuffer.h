#pragma once

#include <memory>

template<typename T>
struct BackBuffer {
    std::unique_ptr<T> pBackBuffer[2] = { nullptr,nullptr };
    T* GetFirst() const { return pBackBuffer[0].get(); }
    T* GetSecond() const { return pBackBuffer[1].get(); }

    T* pFront = nullptr;
    T* pBack = nullptr;

    // both buffer are construct by same param
    template <class... ParamTypes>
    BackBuffer(ParamTypes&&... args) {
        pBackBuffer[0] = std::make_unique<T>(args...);
        pBackBuffer[1] = std::make_unique<T>(args...);

        pFront = GetFirst();
        pBack = GetSecond();
    }

    void Flip() {
        if ( pFront == GetFirst()) { pFront = GetSecond(); pBack = GetFirst(); }
        else { pFront = GetFirst(); pBack = GetSecond(); }
    }
};