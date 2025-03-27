#pragma once

#define NOMINMAX
#include <Windows.h>

struct WaitTimer {
	LARGE_INTEGER performanceCount = {};
	LARGE_INTEGER performanceFrequency = {};

	WaitTimer() {
		QueryPerformanceCounter(&performanceCount);
		QueryPerformanceFrequency(&performanceFrequency);
	}
};