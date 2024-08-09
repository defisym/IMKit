#pragma once

#include <limits>
#include <SDL_mixer.h>

#include "../../Src/Module/General/Definition.h"

struct DataConverter {
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

	// in ms
	static uint32_t GetSampleCount(const size_t duration) {
		const auto sampleCount = static_cast<uint32_t>(static_cast<double>(duration) / 1000.0 * MIX_DEFAULT_FREQUENCY);

		return sampleCount;
	}

	// Convert data for AUDIO_S16SYS, signed int16_t
	// Source is single channel
	// https://stackoverflow.com/questions/5890499/pcm-audio-amplitude-values
	static uint32_t ConvertData(const SourceInfo& srcInfo, const DestInfo& destInfo) {
		const auto sampleCount = GetSampleCount(srcInfo.duration);
		constexpr auto maxValue = std::numeric_limits<int16_t>::max();

		auto pBuffer = destInfo.pBuffer;
		for (size_t index = 0; index < srcInfo.bufferSz; index++) {
			const auto convertResult = static_cast<int16_t>(srcInfo.pBuffer[index] * maxValue);

			for (uint32_t repIdx = 0;
				repIdx < MIX_DEFAULT_CHANNELS; // both channels are the same
				repIdx++, pBuffer++) {
				*pBuffer = convertResult;
			}
		}

		return sampleCount;
	}

	DestInfo _destInfo = {};

	~DataConverter() {
		delete[] _destInfo.pBuffer;
	}

	void ConvertData(const SourceInfo& srcInfo) {
		const auto sampleCount = GetSampleCount(srcInfo.duration);
		const auto bufferSz = sampleCount * MIX_DEFAULT_CHANNELS;

		do {
			if (bufferSz <= _destInfo.bufferSz) { break; }

			delete[] _destInfo.pBuffer;
			_destInfo.pBuffer = new int16_t[bufferSz];
			memset(_destInfo.pBuffer, 0, sizeof(int16_t) * bufferSz);
		} while (false);

		_destInfo.bufferSz = bufferSz;
		ConvertData(srcInfo, _destInfo);
	}
};
