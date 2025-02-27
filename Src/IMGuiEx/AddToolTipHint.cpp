#include "AddToolTipHint.h"

#include <format>

AddToolTipHint::AddToolTipHint(const char* pLabel)
    :StringResult(std::format("{}(?)", pLabel)) {
}

AddToolTipHint::AddToolTipHint(const std::string& label)
    :AddToolTipHint(label.c_str()) {
}
