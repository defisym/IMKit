#include "Internationalization.h"

#include <_3rdLib/JsonInterface.h>
#include <_DeLib/GeneralDefinition.h>

Internationalization::Internationalization() {
    JsonInterface inf = {};
    inf.SetComment(true);

    FILE* fp = nullptr;
    const auto err = fopen_s(&fp, "Internationalization\\Internationalization.json", "rb");
    if (err != 0 || fp == nullptr) { return; }

    inf.Load(fp);
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
        if (tokenMap.contains(tokenName)) { continue; }

        const auto& language = token["language"];
        auto& languageMap = tokenMap[tokenName];

        for (auto& langs : language) {
            const std::string langNameSrc = langs.begin().key();
            std::string langName = ToLower(langNameSrc);
            const auto& langContent = langs[langName];

            if (langName.empty()) { continue; }
            if (languageMap.contains(langName)) { continue; }
            if (std::ranges::find(supportLang, langName) == supportLang.end()) { continue; }
            if (langContent.empty()) { continue; }

            languageMap[langName] = langContent;
        }
    }
}

void Internationalization::UpdateLanguage(const std::string& lang) {
    curLang = ToLower(lang);
}

const std::string& Internationalization::GetInternationalization(const std::string& token) const {
    const auto lowerToken = ToLower(token);
    const auto& tokenIt = tokenMap.find(lowerToken);

    if (tokenIt == tokenMap.end()) { return token; }

    const auto& languageMap = tokenIt->second;
    const auto& languageIt = languageMap.find(curLang);
    if (languageIt == languageMap.end()) { return token; }

    return languageIt->second;
}
