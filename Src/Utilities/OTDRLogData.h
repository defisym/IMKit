#pragma once

#include <_DeLib/CSVHandler.h>

#include "Logger.h"
#include "../Src/DLL/include/OTDR.h"

struct OTDRData {
    const OTDRProcessValueType* pData = nullptr;
    size_t sz = 0u;
};

class OTDRLogData final :public LogData {
    OTDRData logData;
    CSV::CSVBuilder<std::string> builder;
    std::string compressed;

public:
    OTDRLogData(const LogDataConfig& conf = {});

    const OTDRData& GetData() const { return logData; }
    void UpdateData(const OTDRData& data);
    [[nodiscard]] const std::string& ToString() override;

    // uploadaRate: 250M, etc
    // dur: in ms
    // frame: for accumulate
    // return: in KB
    [[nodiscard]] size_t GetEsitimateSize(const size_t uploadaRate, const size_t scanRate,
        const size_t frame, const size_t dur) const;
};