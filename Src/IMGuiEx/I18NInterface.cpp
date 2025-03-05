#include "I18NInterface.h"

#define NOMINMAX
#include <GeneralDefinition.h>

#include "LabelMaker.h"

I18NInterface i18nInf = {};

std::wstring I18NInterface::to_wide_string(const std::string& input) {
    return ::to_wide_string(input);
}

std::string I18NInterface::to_byte_string(const std::wstring& input) {
    return ::to_byte_string(input);
}

StringResult I18NInterface::GetI18NLabel(const char* displayName) const {
    return GetInternationalization(displayName);
}

StringResult I18NInterface::GetI18NLabel(const char* displayName, const char* label) const {
    return ConnectLabel(GetInternationalization(displayName).c_str(), label);
}
