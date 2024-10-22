#include "LabelMaker.h"

#include <format>

#include "imguiInterface/IMGUIContext.h"

LabelMaker::LabelMaker(IMGUIContext* p) :pCtx(p) {}
const std::string& LabelMaker::MakeLabel(const char* displayName) const {
    return pCtx->i18n.GetInternationalization(displayName);
}

const std::string& LabelMaker::MakeLabel(const char* displayName, const char* label) {
    cache = std::format("{}##{}", pCtx->i18n.GetInternationalization(displayName), label);
    return cache;
}
