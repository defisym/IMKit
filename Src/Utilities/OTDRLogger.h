#pragma once

#include "Logger.h"

#include <_DeLib/CSVHandler.h>

struct OTDRData {
    OTDRProcessValueType* pData = nullptr;
    size_t sz = 0u;
};

class OTDRLogger :public LogData {
    OTDRData logData;
    CSV::CSVBuilder<std::string> builder;
    std::string compressed;

public:
    OTDRLogger(const LogDataConfig& conf = {});

    void UpdateData(const OTDRData& data);
    [[nodiscard]] const std::string& ToString() override;
};