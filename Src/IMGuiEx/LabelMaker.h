#pragma once

#include <string>

#include "Utilities/StringResult.h"

struct IMGUIContext;

// imgui will display content before ##, which can be the same
// but after part as label must be unique
class LabelMaker {
    IMGUIContext* pCtx = nullptr;

public:
    LabelMaker(IMGUIContext* p);

    StringResult MakeLabel(const char* displayName) const;
    StringResult MakeLabel(const char* displayName, const char* label) const;

    std::string MakeLabelStr(const char* displayName) const;
    std::string MakeLabelStr(const char* displayName, const char* label) const;
    // no localization
    static std::string ConnectLabel(const char* displayName, const char* label);
};

