#pragma once

#include "Logger.h"

#include <_DeLib/CSVHandler.h>

struct OTDRData {
    OTDRProcessValueType* pData = nullptr;
    size_t sz = 0u;
};

class OTDRLogger final :public LogData {
    OTDRData logData;
    CSV::CSVBuilder<std::string> builder;
    std::string compressed;

public:
    OTDRLogger(const LogDataConfig& conf = {});

    void UpdateData(const OTDRData& data);
    [[nodiscard]] const std::string& ToString() override;

    // uploadaRate: 250M, etc
    // dur: in ms
    // frame: for accumulate
    // return: in KB
    [[nodiscard]] size_t GetEsitimateSize(const size_t uploadaRate, const size_t scanRate,
        const size_t frame, const size_t dur) const;
};