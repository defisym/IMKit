#pragma once

#include <utility>

#include "Internationalization/Internationalization.h"
#include "Utilities/StringResult.h"
#include "IMGuiEx/LabelMaker.h"

struct I18NInterface;
extern I18NInterface i18nInf;

#define I18N(...) i18nInf.GetI18NLabel(__VA_ARGS__)
#define I18NFMT(...) i18nInf.GetI18NFMT(__VA_ARGS__)
#define I18NFMTL(...) i18nInf.GetI18NFMTLabel(__VA_ARGS__)

struct I18NInterface :Internationalization {
private:
    static std::string to_byte_string(const std::wstring& input);
    static std::wstring to_wide_string(const std::string& input);

    static auto to_wide_string_if_needed(const auto& arg) -> decltype(auto) {
        using Type = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<std::string, Type>
            || std::is_constructible_v<std::string, Type>) {
            return to_wide_string(arg);
        }
        else {
            return std::forward<decltype(arg)>(arg);
        }
    }

    template <class... Types>
    StringResult GetI18NFMTInternal(const std::wstring_view& fmt, Types&&... args) {
        return to_byte_string(std::vformat(fmt, std::make_wformat_args(args...)));
    }

public:
    StringResult GetI18NLabel(const char* displayName) const;
    StringResult GetI18NLabel(const char* displayName, const char* label) const;

    template <class... Types>
    StringResult GetI18NFMT(const char* pFmt, Types&&... args) {     
        // std::make_wformat_args expects lvalue reference
        // if pass it directly, the return of to_wide_string_if_needed is rvalue
        // so nest it by GetI18NLabelFMTInternal
        // as function param is a lvalue
        return GetI18NFMTInternal(to_wide_string(GetInternationalization(pFmt)),
            to_wide_string_if_needed(std::forward<Types>(args))...);
    }
    template <class... Types>
    StringResult GetI18NFMTLabel(const char* pFmt, const char* label, Types&&... args) {
        const auto fmt = GetI18NFMT(pFmt, std::forward<Types>(args)...);
        return ConnectLabel(fmt, label);
    }
};
