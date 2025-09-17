#include "AudioHandler.h"

#include "macro.h"

AudioHandler::AudioHandler(const size_t duration)
    :audioData(DataConverter::GetBufferCount(duration)) {
#ifndef NO_AUDIO
    AudioPlayer::StartAudio(audioData);
#endif
}

void AudioHandler::ResetBuffer() {
    audioData.ringBuffer.Reset();
}

void AudioHandler::ResetIndex() {
    audioData.ringBuffer.ResetIndex();
}

void AudioHandler::AddData(const DataConverter::SourceInfo& sourceInfo) {
    audioData.AddData(sourceInfo);
}
