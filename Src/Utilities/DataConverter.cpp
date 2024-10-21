#include "DataConverter.h"

#include <limits>
#include <cassert>

uint32_t DataConverter::ConvertData(const SourceInfo& srcInfo, const DestInfo& destInfo) {
    constexpr auto maxValue = std::numeric_limits<int16_t>::max();
    const auto sampleCount = GetSampleCount(srcInfo.duration);
    const auto repeat = static_cast<double>(sampleCount) / static_cast<double>(srcInfo.bufferSz);
    auto remain = repeat;
#ifdef _DEBUG
    uint32_t count = 0;
#endif

    auto pBuffer = destInfo.pBuffer;
    auto copyData = [&] (const int16_t convertResult) {
        while (remain >= 1.0) {
            // both channels have the same content
            for (size_t channelIdx = 0; channelIdx < MIX_DEFAULT_CHANNELS; channelIdx++, pBuffer++) {
                *pBuffer = convertResult;
#ifdef _DEBUG
                count++;
#endif
            }
            remain -= 1.0;
        }

        remain += repeat;
    };

    for (size_t index = 0; index < srcInfo.bufferSz; index++) {
        copyData(static_cast<int16_t>(srcInfo.pBuffer[index] * maxValue));
    }

#ifdef _DEBUG
    constexpr auto tolerance = 5;
    assert(abs(sampleCount * MIX_DEFAULT_CHANNELS - count) < tolerance);
#endif

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
