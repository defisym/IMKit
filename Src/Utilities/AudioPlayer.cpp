#include "AudioPlayer.h"

#include <format>

#include <SDL.h>
#include <SDL_mixer.h>

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2_mixer_ext.lib")

#include "macro.h"

AudioData::AudioChunk::AudioChunk() {
    pChunk = Mix_QuickLoad_RAW(chunk, CHUNK_SIZE);
    if (pChunk) { return; }

    throw std::exception(std::format("{}", SDL_GetError()).c_str());
}

AudioData::AudioChunk::~AudioChunk() {
    Mix_FreeChunk(pChunk);
}

void AudioData::AddData(const DataConverter::SourceInfo& sourceInfo) {
#ifndef NO_AUDIO
    dataConverter.ConvertData(sourceInfo);
    const auto convBuf = dataConverter._destInfo;

    ringBuffer.WriteData(convBuf.pBuffer, convBuf.bufferSz);
#endif
}

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
