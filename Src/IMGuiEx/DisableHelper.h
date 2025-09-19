#pragma once

// RAII
struct DisableHelper {  // NOLINT(cppcoreguidelines-special-member-functions)
    bool bDisabled = false;

    DisableHelper(const bool bDisabled);
    ~DisableHelper();
};


struct ManualDisableHelper {  // NOLINT(cppcoreguidelines-special-member-functions)
    DisableHelper* pDisableHelper = nullptr;
    ~ManualDisableHelper() { Enable(); }

    void Disable(const bool bDisabled);
    void Enable();
};
