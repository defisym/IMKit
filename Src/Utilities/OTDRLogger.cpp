#include "OTDRLogger.h"
#include <_3rdLib/Compress/zlibInterface.h>

OTDRLogger::OTDRLogger(const LogDataConfig& config):LogData(config) {}

void OTDRLogger::UpdateData(const OTDRData& data) {
    this->logData = data;
}

std::string OTDRLogger::ToString() {
    std::string* pResult = &builder.result;
    builder.Reset();

    if (!config.bBinary) {
        for (size_t index = 0; index < logData.sz; index++) {
            builder.AddNewItem(_ftos<std::string>(logData.pData[index]));
        }
    }else {
        const auto binSz = sizeof(OTDRProcessValueType) * logData.sz;
        pResult->resize(binSz);
        memcpy(pResult->data(), logData.pData, binSz);
    }

    if (!config.bCompress) { return *pResult; }

    const auto sz = compressBound(static_cast<uLong>(pResult->size()));
    compressed.clear();
    compressed.resize(sz, '\0');

    uLong compressedSize = sz;

    const auto ret = compress2(reinterpret_cast<Bytef*>(compressed.data()),
        &compressedSize,
        reinterpret_cast<Bytef*>(pResult->data()),
        static_cast<uLong>(pResult->size()),
        Z_DEFAULT_COMPRESSION);

    if (ret != Z_OK) { compressedSize = 0; }

    compressed.resize(compressedSize);

    return compressed;
}
