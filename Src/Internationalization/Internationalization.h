#pragma once

#include <map>
#include <string>
#include <vector>

struct Internationalization {
    std::string curLang = "zh-cn";
    std::vector<std::string> supportLang;
    std::map<std::string, std::map<std::string, std::string>> tokenMap;

    Internationalization();
    void UpdateLanguage(const std::string& lang);
    const std::string& GetInternationalization(const std::string& token) const;
};