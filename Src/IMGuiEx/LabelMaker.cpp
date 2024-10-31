#include "LabelMaker.h"

#include <format>

#include "imguiInterface/IMGUIContext.h"

LabelMaker::LabelMaker(IMGUIContext* p) :pCtx(p) {}

const std::string& LabelMaker::UpdateCache(const std::string& str) {
    cache = str;
    return cache;
}

const std::string& LabelMaker::MakeLabel(const char* displayName) {
    cache = MakeLabelStr(displayName);
    return cache;
}

const std::string& LabelMaker::MakeLabel(const char* displayName, const char* label) {
    cache = MakeLabelStr(displayName, label);
    return cache;
}

std::string LabelMaker::MakeLabelStr(const char* displayName) const {
    return pCtx->i18n.GetInternationalization(displayName);
}

std::string LabelMaker::MakeLabelStr(const char* displayName, const char* label) const {
    return ConnectLabel(pCtx->i18n.GetInternationalization(displayName).c_str(), label);
}

std::string LabelMaker::ConnectLabel(const char* displayName, const char* label) {
    return std::format("{}##{}", displayName, label);
}
