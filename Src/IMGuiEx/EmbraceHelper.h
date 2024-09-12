#pragma once

// For the case logic should run no matter tab is opened or not
// if Tab bar opened, use RAII to close it

struct EmbraceHelper {  // NOLINT(cppcoreguidelines-special-member-functions)
    using ExitFuncType = void(*)();

    bool _bOpen = false;
    ExitFuncType _pExit = nullptr;

    EmbraceHelper(const bool bOpen, const ExitFuncType pExit = nullptr)
        :_bOpen(bOpen), _pExit(pExit) {}
    ~EmbraceHelper() {
        if (_bOpen && _pExit != nullptr) { _pExit(); }
    }
};
