#pragma once

#include <map>
#include <string>

#include "imgui.h"

struct FontInfo {
    std::string name;
    float size = 0.0f;

    size_t GetHash() const;
};

using FontInfoHash = size_t;

class FontEx {
    ImFontAtlas* pFontAtlas = nullptr;

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;

    ImFontConfig conf = {};

    std::map<FontInfoHash, ImFont*> fontLib;

public:
    FontEx();

    bool AddFont(const char* filename, float size_pixels);
#ifdef _DEBUG
    void FontDebug();
#endif
};