#pragma once

#include <functional>

#include "IMGUIContext.h"

int IMGUIInterface(IMGUIContext* pCtx,
                   const std::function<void(IMGUIContext*)>& gui);
