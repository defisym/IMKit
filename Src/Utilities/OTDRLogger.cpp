#include "OTDRLogger.h"

#include <_3rdLib/Compress/zlibInterface.h>

OTDRLogger::OTDRLogger(const LogDataConfig& conf):LogData(conf) {}

void OTDRLogger::UpdateData(const OTDRData& data) {
    this->logData = data;
}

const std::string& OTDRLogger::ToString() {
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

// estimate formulat: point num / 400 (compressed) or 200 (uncompressed)
size_t OTDRLogger::GetEsitimateSize(const size_t uploadaRate, const size_t scanRate,
        const size_t frame, const size_t dur) const {
    const auto processTimes = static_cast<size_t>(std::floor((static_cast<double>(dur) / 1000.0) * static_cast<double>(scanRate) / static_cast<double>(frame)));
    const auto dataPerScan = static_cast<size_t>(std::floor(static_cast<double>(uploadaRate) / static_cast<double>(scanRate)));
    const auto totalPointNum = processTimes * dataPerScan + processTimes * frame;
    const auto dataPerSec = totalPointNum / (config.bCompress ? 400 : 200);

    return dataPerSec;
}
