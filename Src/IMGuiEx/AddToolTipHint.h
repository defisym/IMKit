#pragma once

#include "../Utilities/StringResult.h"

struct AddToolTipHint : StringResult {
    AddToolTipHint(const char* pLabel);
    AddToolTipHint(const std::string& label);
};
