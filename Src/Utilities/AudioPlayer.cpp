#include "AudioPlayer.h"

#include <SDL.h>
#include <SDL_mixer.h>

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2_mixer_ext.lib")


bool AudioPlayer::InitAudio() {
	if (!SDL_WasInit(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
			[[maybe_unused]] auto error = SDL_GetError();

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
			[[maybe_unused]] auto error = SDL_GetError();

			return false;
		}
	}

	Mix_AllocateChannels(MIX_CHANNELS);

	return true;
}

void AudioPlayer::CloseAudio() {
	Mix_CloseAudio();
	SDL_Quit();
}

void AudioPlayer::StartAudio(AudioData& audioData) {
	const auto channel = Mix_PlayChannel(-1, audioData.audioChunk.pChunk, -1);
	Mix_RegisterEffect(channel,
	   [] (int chan, void* stream, int len, void* udata) {
		   const auto pAudioData = static_cast<AudioData*>(udata);
		   SDL_memset(stream, 0, len);
		   pAudioData->ringBuffer.ReadData(static_cast<int16_t*>(stream), len / sizeof(int16_t));
	   },
	   [] (int chan, void* udata) {
		   //OutputDebugString(L"Finish");
	   },
	   &audioData);
}

void AudioPlayer::AddData(AudioData& audioData, const DataConverter::SourceInfo& sourceInfo) {
	dataConverter.ConvertData(sourceInfo);
	const auto convBuf = dataConverter._destInfo;

	audioData.ringBuffer.WriteData(convBuf.pBuffer, convBuf.bufferSz);
}
