#include "IMGUIContext.h"

const char* IMGUIContext::GetI18NLabel(const char* displayName) const {    
    return labelMaker.MakeLabel(displayName).c_str();
}

const char* IMGUIContext::GetI18NLabel(const char* displayName, const char* label) {
    return labelMaker.MakeLabel(displayName, label).c_str();
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
