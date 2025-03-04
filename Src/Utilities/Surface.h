#pragma once

#include "imguiInterface/IMGUIContext.h"

class SurfaceFactory {
    const D3DContext* pD3DCtx = nullptr;

    SurfaceFactory(const D3DContext* pCtx);
};
