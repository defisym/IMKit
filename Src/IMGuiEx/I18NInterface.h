#pragma once

#include "Internationalization/Internationalization.h"
#include "Utilities/StringResult.h"

struct I18NInterface;
extern I18NInterface i18nInf;

#define I18N(...) i18nInf.GetI18NLabel(__VA_ARGS__)
#define I18NFMT(...) i18nInf.GetI18NLabelFMT(__VA_ARGS__)

struct I18NInterface :Internationalization {
private:
    std::wstring to_wide_string(const std::string& input);
    std::string to_byte_string(const std::wstring& input);

public:
    StringResult GetI18NLabel(const char* displayName) const;
    StringResult GetI18NLabel(const char* displayName, const char* label) const;

    template <class... Types>
    StringResult GetI18NLabelFMT(const char* displayName, Types&&... args) {
        const auto fmt = std::vformat(to_wide_string(GetInternationalization(displayName)),
            std::make_wformat_args(std::forward<Types>(args)...));
        return to_byte_string(fmt);
    }
    template <class... Types>
    StringResult GetI18NLabelFMT(const char* displayName, const char* label, Types&&... args) {
        const auto fmt = GetI18NLabelFMT(displayName, std::forward<Types>(args)...);
        return ConnectLabel(fmt, label);
    }
};
