#include "Internationalization.h"

#include <JsonInterface.h>
#include <GeneralDefinition.h>

#include "macro.h"

Internationalization::Internationalization() {
#ifndef NO_I18N
    JsonInterface inf = {};
    inf.SetComment(true);

    FILE* fp = nullptr;
    const auto err = fopen_s(&fp, "Internationalization\\Internationalization.json", "rb");
    if (err != 0 || fp == nullptr) { return; }

    const auto bValid = inf.Load(fp); fclose(fp);
    if (!bValid) { return; }

    const auto& data = inf.Get();
    const auto& langArray = JsonInterface::GetData(data, "language");
    for (auto& it : langArray) { supportLang.emplace_back(it); }
    for (auto& it : supportLang) { ToLower(it); }

    const auto& tokens = JsonInterface::GetData(data, "internationalization");
    for(auto& token: tokens) {
        if (!token.contains("token")) { continue; }
        if (!token.contains("language")) { continue; }

        const std::string tokenNameSrc = token["token"];
        std::string tokenName = ToLower(tokenNameSrc);
        if (tokenName.empty()) { continue; }
        if (tokenMap.contains(tokenName)) {
#ifdef _DEBUG
            OutputDebugStringA(std::format("Duplicate token: {}\n",
                tokenNameSrc).c_str());
#endif
            continue;
        }

        const auto& language = token["language"];
        auto& languageMap = tokenMap[tokenName];

        for (auto& langs : language) {
            const std::string langNameSrc = langs.begin().key();
            std::string langName = ToLower(langNameSrc);
            const auto& langContent = langs[langName];

            if (langName.empty()) { continue; }
            if (languageMap.contains(langName)) {
#ifdef _DEBUG
                OutputDebugStringA(std::format("Duplicate token {} localization: {}\n",
                    tokenNameSrc, langNameSrc).c_str());
#endif
                continue;
            }
            if (std::ranges::find(supportLang, langName) == supportLang.end()) { continue; }
            if (langContent.empty()) { continue; }

            languageMap[langName] = langContent;
        }
    }
#endif
}

void Internationalization::UpdateLanguage(const std::string& lang) {
#ifndef NO_I18N
    curLang = ToLower(lang);
    bCurLangDefault = curLang == LANGUAGE_INTERNAL;
#else
    bCurLangDefault = true;
#endif    
}

const std::string& Internationalization::GetInternationalization(const std::string& token) const {
    // no process needed in English
    if (bCurLangDefault) { return token; }

    const auto lowerToken = ToLower(token);
    const auto& tokenIt = tokenMap.find(lowerToken);

    if (tokenIt == tokenMap.end()) { return token; }

    const auto& languageMap = tokenIt->second;
    const auto& languageIt = languageMap.find(curLang);
    if (languageIt == languageMap.end()) { return token; }

    return languageIt->second;
}
