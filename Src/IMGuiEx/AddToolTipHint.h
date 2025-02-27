#pragma once
#include <string>

struct AddToolTipHint {
    std::string label;

    AddToolTipHint(const char* pLabel);
    operator const char* () const;
};
