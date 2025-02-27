#include "IMGUIContext.h"

StringResult IMGUIContext::GetI18NLabel(const char* displayName) const {
    return labelMaker.MakeLabel(displayName);
}

StringResult IMGUIContext::GetI18NLabel(const char* displayName, const char* label) const {
    return labelMaker.MakeLabel(displayName, label);
}

bool IMGUIContext::InitContext() {
    pFont = new FontEx{};

    return true;
}

bool IMGUIContext::UnInitContext() {
    delete pFont;
    pFont = nullptr;

    return true;
}
