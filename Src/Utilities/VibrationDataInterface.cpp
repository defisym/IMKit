#include "VibrationDataInterface.h"

#include <_3rdLib/Compress/zlibInterface.h>

VibrationDataInterface::VibrationDataInterface(const LogDataConfig& conf) :LogDataInterface(conf) {}

void VibrationDataInterface::UpdateData(const OTDRData& data) {
    this->logData = data;
}

const std::string& VibrationDataInterface::ToString() {
    const auto& str = stringify.ToString(logData, config.bBinary);
    if (!config.bCompress) { return str; }

    const auto sz = compressBound(static_cast<uLong>(str.size()));
    compressed.clear();
    compressed.resize(sz, '\0');

    uLong compressedSize = sz;

    const auto ret = compress(reinterpret_cast<Bytef*>(compressed.data()),
        &compressedSize,
        reinterpret_cast<const Bytef*>(str.data()),
        static_cast<uLong>(str.size()));

    if (ret != Z_OK) { compressedSize = 0; }

    compressed.resize(compressedSize);

    return compressed;
}