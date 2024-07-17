#pragma once
#include <string>

#include "../../Src/Module/General/Definition.h"

// PCM data:
//  16 bit mono:
//      | high | low | high | low |...
//  16 bit stereo:
//      | ch0 high | ch0 low | ch1 high | ch1 low |...
// PCM data range:
//  Signed 16 bit: -32768 ~ 32767
// Sample rate:
//  Point per second
struct AudioPlayer {
    inline static bool InitAudio();

	struct SourceInfo {
        // pBuffer should be normalized to -1.0f ~ 1.0f
        OTDRProcessValueType* pBuffer;
    	size_t bufferSz;
        // in ms
        size_t duration;
    };

    struct DestInfo {
        int16_t* pBuffer;
        size_t bufferSz;
    };

    inline static void ConvertData(const SourceInfo& srcInfo, const DestInfo& destInfo);

    struct AudioInfo {
        std::string audioName;

    };
};