#include "IMGuiContext.h"

bool IMGUIContext::InitContext() {
    pFont = new FontEx{};

    return true;
}

bool IMGUIContext::UnInitContext() {
    delete pFont;
    pFont = nullptr;

    return true;
}
