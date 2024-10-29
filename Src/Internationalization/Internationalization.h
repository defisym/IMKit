#pragma once

#include <map>
#include <string>
#include <vector>

constexpr auto LANGUAGE_LENGTH = 16;
constexpr auto LANGUAGE_CHINESE = "zh-cn";
constexpr auto LANGUAGE_ENGLISH = "en-us";

// language used in source code
constexpr auto LANGUAGE_INTERNAL = LANGUAGE_ENGLISH;

struct Internationalization {
    bool bCurLangDefault = true;
    std::string curLang = LANGUAGE_ENGLISH;
    std::vector<std::string> supportLang;
    std::map<std::string, std::map<std::string, std::string>> tokenMap;

    Internationalization();
    void UpdateLanguage(const std::string& lang);
    const std::string& GetInternationalization(const std::string& token) const;
};