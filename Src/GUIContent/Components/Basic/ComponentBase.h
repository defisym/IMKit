#pragma once

#include "GUIContext/OTDRGUIContext.h"

struct ComponentBase {  // NOLINT(cppcoreguidelines-special-member-functions)
	Ctx* pCtx = nullptr;

	explicit ComponentBase(Ctx* p) :pCtx(p) {}
	~ComponentBase() = default;
};