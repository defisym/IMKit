#pragma once

#include <functional>

#include "IMGuiContext.h"

int IMGUIInterface(IMGUIContext* pCtx,
                   const std::function<void(IMGUIContext*)>& gui);
