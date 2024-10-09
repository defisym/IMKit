#include "OTDRLogger.h"

OTDRLogger::OTDRLogger(const LogDataConfig& config):LogData(config) {}

void OTDRLogger::UpdateData(const OTDRData& data) {
    this->logData = data;
}

std::string OTDRLogger::ToString() {
    builder.Reset();
    ret.clear();

    for (size_t index = 0; index < logData.sz; index++) {
        builder.AddNewItem(_ftos(logData.pData[index]));
    }

    const auto resultSz = sizeof(wchar_t) * builder.result.size();
    ret.resize(resultSz, '\0');
    memcpy(ret.data(), builder.result.data(), resultSz);

    return ret;
}
