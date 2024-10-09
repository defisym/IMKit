#include "OTDRLogger.h"

OTDRLogger::OTDRLogger(const LogDataConfig& config):LogData(config) {}

void OTDRLogger::UpdateData(const OTDRData& data) {
    this->logData = data;
}

std::string OTDRLogger::ToString() {
    builder.Reset();

    for (size_t index = 0; index < logData.sz; index++) {
        builder.AddNewItem(std::format("{}", logData.pData[index]));
    }

    return builder.result;
}
