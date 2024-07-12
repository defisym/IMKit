#include "AudioPlayer.h"

#include <SDL.h>
#include <SDL_mixer.h>

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
		if (Mix_OpenAudioDevice(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, SDL_BUFFER_SIZE,
								nullptr, 0) == -1) {
			auto error = SDL_GetError();

			return false;
		}
	}

	Mix_AllocateChannels(MIX_CHANNELS);

	return true;
}
