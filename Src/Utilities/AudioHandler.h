#pragma once

#include "Utilities/AudioPlayer.h"

struct AudioHandler {  // NOLINT(cppcoreguidelines-special-member-functions)
    AudioPlayer audioPlayer;
    AudioData audioData;

    static constexpr size_t DEFAULT_DURATION = 4096;

    // allocate audio buffer in audio data struct
    // of given duration (in ms), default is 4096
    // TODO: maybe not large enough
    AudioHandler(const size_t duration = DEFAULT_DURATION);
    ~AudioHandler() = default;

    void ResetBuffer();
    void ResetIndex();
    void AddData(const DataConverter::SourceInfo& sourceInfo);
};
