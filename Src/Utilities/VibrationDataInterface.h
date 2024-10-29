#pragma once

#include "Logger.h"
#include "OTDRDataInterface.h"

class VibrationDataInterface :public LogDataInterface {
    using DataType = OTDRData;

    DataType logData;
    OTDRDataStringify stringify;

public:
    VibrationDataInterface(const LogDataConfig& conf = {});

    void UpdateData(const DataType& data);
    [[nodiscard]] const DataType& GetData() const { return logData; }
    [[nodiscard]] const std::string& ToString() override;
};

static_assert(ValidDataInterface<VibrationDataInterface>);