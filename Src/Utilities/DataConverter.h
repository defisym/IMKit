#pragma once

#include <SDL_mixer.h>

#include "../../Src/Module/General/Definition.h"

struct DataConverter {  // NOLINT(cppcoreguidelines-special-member-functions)
	struct SourceInfo {
		// pBuffer should be normalized to -1.0f ~ 1.0f
		const OTDRProcessValueType* pBuffer;
		size_t bufferSz;
		// in ms
		size_t duration;
	};

	struct DestInfo {
		int16_t* pBuffer;
		size_t bufferSz;
	};

	// in ms
	static constexpr uint32_t GetSampleCount(const size_t duration) {
		return static_cast<uint32_t>(static_cast<double>(duration) / 1000.0 * MIX_DEFAULT_FREQUENCY);
	}

	static constexpr uint32_t GetBufferCount(const size_t duration) {
		return GetSampleCount(duration) * MIX_DEFAULT_CHANNELS;
	}

	// Convert data for AUDIO_S16SYS, signed int16_t
	// Source is single channel
	// https://stackoverflow.com/questions/5890499/pcm-audio-amplitude-values
	static uint32_t ConvertData(const SourceInfo& srcInfo, const DestInfo& destInfo);
    void ConvertData(const SourceInfo& srcInfo);

    DestInfo _destInfo = {};

	~DataConverter();
};
