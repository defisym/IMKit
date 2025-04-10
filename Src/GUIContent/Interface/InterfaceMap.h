#pragma once

#include <functional>

struct TileManager; struct ViewParams;
void InterfaceMap(const char* pID, 
    TileManager* pTileManager, ViewParams* pViewParams,
    const std::function<void()>& extra = nullptr);