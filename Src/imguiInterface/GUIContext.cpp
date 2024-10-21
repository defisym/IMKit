#include "GUIContext.h"

bool GUIContext::InitContext() {
    pFont = new FontEx{};

    return true;
}

bool GUIContext::UnInitContext() {
    delete pFont;
    pFont = nullptr;

    return true;
}
