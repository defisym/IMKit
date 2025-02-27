#include "AddToolTipHint.h"

#include <format>

AddToolTipHint::AddToolTipHint(const char* pLabel) {
    label = std::format("{}(?)", pLabel);
}

AddToolTipHint::operator const char* () const {
    return label.c_str();
}
