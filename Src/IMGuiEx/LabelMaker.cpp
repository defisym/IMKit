#include "LabelMaker.h"

#include <format>

#include "imguiInterface/IMGUIContext.h"

LabelMaker::LabelMaker(IMGUIContext* p) :pCtx(p) {}

StringResult LabelMaker::MakeLabel(const char* displayName) const {
    return MakeLabelStr(displayName);
}

StringResult LabelMaker::MakeLabel(const char* displayName, const char* label) const {
    return MakeLabelStr(displayName, label);
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
