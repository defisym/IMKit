#include "LabelMaker.h"

#include <format>

#include "imguiInterface/IMGUIContext.h"

LabelMaker::LabelMaker(IMGUIContext* p) :pCtx(p) {}

const std::string& LabelMaker::UpdateCache(const std::string& str) {
    cache = str;
    return cache;
}

const std::string& LabelMaker::MakeLabel(const char* displayName) {
    cache = pCtx->i18n.GetInternationalization(displayName);
    return cache;
}

const std::string& LabelMaker::MakeLabel(const char* displayName, const char* label) {
    cache = std::format("{}##{}", pCtx->i18n.GetInternationalization(displayName), label);
    return cache;
}
