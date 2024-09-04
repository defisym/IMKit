// ReSharper disable CppClangTidyCppcoreguidelinesSpecialMemberFunctions
#pragma once

#include <format>
#include <string>

#include <_DeLib/ThreadSafeRingBuffer.h>

#include "DataConverter.h"
#include "../../Src/Module/General/Definition.h"

constexpr auto SDL_BUFFER_SIZE = 4096;
constexpr auto CHUNK_SIZE = 4 * SDL_BUFFER_SIZE;
constexpr auto PCM_BUFFER_SIZE = 4 * CHUNK_SIZE;

struct AudioData {
	struct AudioChunk {
		uint8_t chunk[CHUNK_SIZE] = {};
		Mix_Chunk* pChunk = nullptr;

		AudioChunk() {
			pChunk = Mix_QuickLoad_RAW(chunk, CHUNK_SIZE);
			if (pChunk) { return; }

			throw std::exception(std::format("{}", SDL_GetError()).c_str());
		}
		~AudioChunk() {
			Mix_FreeChunk(pChunk);
		}
	};

	std::string audioName;
	AudioChunk audioChunk;
	ThreadSafeRingBuffer<int16_t> ringBuffer;

	AudioData() :ringBuffer(PCM_BUFFER_SIZE) {}
	explicit AudioData(const size_t sz) :ringBuffer(sz) {}
};

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
    static bool InitAudio();
    static void CloseAudio();

    AudioPlayer() { InitAudio(); }
    ~AudioPlayer() { CloseAudio(); }

    static void StartAudio(AudioData& audioData);

    DataConverter dataConverter = {};
    void AddData(AudioData& audioData, const DataConverter::SourceInfo& sourceInfo);
};