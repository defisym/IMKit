#include "AudioPlayer.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <limits>

constexpr auto SDL_BUFFER_SIZE = 4096;

bool AudioPlayer::InitAudio() {
	if (!SDL_WasInit(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
			auto error = SDL_GetError();

			return false;
		}
	}

	if (!Mix_AudioOpened()) {
		// If your app needs precisely what is requested, specify zero for `allowed_changes`
		if (Mix_OpenAudioDevice(MIX_DEFAULT_FREQUENCY,
			MIX_DEFAULT_FORMAT,
			MIX_DEFAULT_CHANNELS,
			SDL_BUFFER_SIZE,
			nullptr, 0) == -1) {
			auto error = SDL_GetError();

			return false;
		}
	}

	Mix_AllocateChannels(MIX_CHANNELS);

	return true;
}

// Convert data for AUDIO_S16SYS, signed int16_t
// Source is single channel
// https://stackoverflow.com/questions/5890499/pcm-audio-amplitude-values
void AudioPlayer::ConvertData(const SourceInfo& srcInfo, const DestInfo& destInfo) {
	constexpr auto maxValue = std::numeric_limits<int16_t>::max();
	const auto sampleCount = static_cast<uint16_t>(static_cast<double>(srcInfo.duration) / 1000.0 * MIX_DEFAULT_FREQUENCY);

	auto pBuffer = destInfo.pBuffer;
	for (size_t index = 0; index < srcInfo.bufferSz; index++) {
		const auto convertResult = static_cast<int16_t>(srcInfo.pBuffer[index] * maxValue);
		const auto repeat = sampleCount * 2; // both channels are the same

		for (uint16_t repIdx = 0; repIdx < repeat; repIdx++, pBuffer++) {
			*pBuffer = convertResult;
		}
	}
}
