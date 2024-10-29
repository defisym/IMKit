#include "OTDRDataInterface.h"

const std::string& OTDRDataStringify::ToString(const OTDRData& data, const bool bBinary) {
    builder.Reset();

    if (!bBinary) {
        for (size_t index = 0; index < data.sz; index++) {
            builder.AddNewItem(_ftos<std::string>(data.pData[index]));
        }
    }
    else {
        const auto binSz = sizeof(OTDRProcessValueType) * data.sz;
        builder.result.resize(binSz);
        memcpy(builder.result.data(), data.pData, binSz);
    }

    return builder.result;

}

