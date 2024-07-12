#pragma once

// PCM data:
// 16 bit mono:
//  | high | low | high | low |...
// 16 bit stereo:
//  | ch0 high | ch0 low | ch1 high | ch1 low |...
struct AudioPlayer {
    inline static bool InitAudio();
};