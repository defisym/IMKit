#include "IMGUIContext.h"

const char* IMGUIContext::GetI18NLabel(const char* displayName) {    
    return labelMaker.MakeLabel(displayName).c_str();
}

const char* IMGUIContext::GetI18NLabel(const char* displayName, const char* label) {
    return labelMaker.MakeLabel(displayName, label).c_str();
}

std::string IMGUIContext::GetI18NLabelStr(const char* displayName) {
    return labelMaker.MakeLabelStr(displayName);
}

std::string IMGUIContext::GetI18NLabelStr(const char* displayName, const char* label) {
    return labelMaker.MakeLabelStr(displayName, label);
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
