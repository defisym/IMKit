#include "DataConverter.h"

uint32_t DataConverter::ConvertData(const SourceInfo& srcInfo, const DestInfo& destInfo) {
    constexpr auto maxValue = std::numeric_limits<int16_t>::max();
    const auto sampleCount = GetSampleCount(srcInfo.duration);
    const auto repeat = sampleCount / srcInfo.bufferSz;
    auto remain = sampleCount % srcInfo.bufferSz;

    auto pBuffer = destInfo.pBuffer;
    auto copyData = [&] (const int16_t convertResult, const size_t repeatTimes) {
        const auto copySz = repeatTimes * MIX_DEFAULT_CHANNELS; // both channels have the same content

        for (size_t repIdx = 0; repIdx < copySz; repIdx++, pBuffer++) {
            *pBuffer = convertResult;
        }
    };
    auto updateRemain = [&] () {
        if (remain == 0) { return 0; }

        remain--;
        return 1;
    };

    for (size_t index = 0; index < srcInfo.bufferSz; index++) {
        copyData(static_cast<int16_t>(srcInfo.pBuffer[index] * maxValue),
            repeat + updateRemain());
    }

    return sampleCount;
}

void DataConverter::ConvertData(const SourceInfo& srcInfo) {
    const auto bufferSz = GetBufferCount(srcInfo.duration);

    do {
        if (bufferSz <= _destInfo.bufferSz) { break; }

        delete[] _destInfo.pBuffer;
        _destInfo.pBuffer = new int16_t[bufferSz];
        memset(_destInfo.pBuffer, 0, sizeof(int16_t) * bufferSz);
    } while (false);

    _destInfo.bufferSz = bufferSz;
    ConvertData(srcInfo, _destInfo);
}

DataConverter::~DataConverter() {
    delete[] _destInfo.pBuffer;
}
