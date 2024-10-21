#pragma once

#include <functional>

#include "GUIContext.h"

int ImguiInterface(GUIContext* pCtx,
	const std::function<void(GUIContext*)>& gui);
