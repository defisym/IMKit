#pragma once

#include "Logger.h"

#include <_DeLib/CSVHandler.h>

struct OTDRData {
    OTDRProcessValueType* pData = nullptr;
    size_t sz = 0u;
};

class OTDRLogger :public LogData {
    OTDRData logData;
    CSVBuilder builder;
    std::string ret;

public:
    OTDRLogger(const LogDataConfig& config = {});

    void UpdateData(const OTDRData& data);
    [[nodiscard]] std::string ToString() override;
};