#pragma once

#include "Logger.h"
#include "OTDRDataInterface.h"

class VibrationDataInterface :public LogDataInterface {
    OTDRData logData;
    std::string compressed;
    OTDRDataStringify stringify;

public:
    VibrationDataInterface(const LogDataConfig& conf = {});

    void UpdateData(const OTDRData& data);
    [[nodiscard]] const OTDRData& GetData() const { return logData; }
    [[nodiscard]] const std::string& ToString() override;
};

static_assert(ValidDataInterface<VibrationDataInterface>);