#pragma once

#include <string>

struct IMGUIContext;

// imgui will display content before ##, which can be the same
// but after part as label must be unique
class LabelMaker {
    IMGUIContext* pCtx = nullptr;
    std::string cache;

public:
    LabelMaker(IMGUIContext* p);
    const std::string& UpdateCache(const std::string& str);
    const std::string& MakeLabel(const char* displayName);
    const std::string& MakeLabel(const char* displayName, const char* label);
    std::string MakeLabelStr(const char* displayName) const;
    std::string MakeLabelStr(const char* displayName, const char* label) const;
    // no localization
    static std::string ConnectLabel(const char* displayName, const char* label);
};

