//#define USE_CONSOLE

#ifdef USE_CONSOLE
#pragma warning(disable : 4996)
#include <iostream>
#endif

#include <complex>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include "./../../Src/DLL/include/OTDR.h"
#include "./../Utilities/AudioPlayer.h"
#include "./../GUIContext/Handler/AudioHandler.h"

namespace AudioTest {
    //constexpr auto DEFAULT_BUFFER_SZ = 44100; 
    constexpr auto DEFAULT_BUFFER_SZ = 2560;
    constexpr auto DEFAULT_FREQ = 200.0;
    constexpr auto DEFAULT_DURA = 1000;

    const OTDRProcessValueType* AllocBuffer(const size_t sz = DEFAULT_BUFFER_SZ) {
#ifdef USE_CONSOLE
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#endif
        const auto p = new OTDRProcessValueType[sz];
        const auto dt = 1.0 / static_cast<double>(sz);

        for (size_t index = 0; index < sz; index++) {
            p[index] = static_cast<OTDRProcessValueType>(std::sin(2 * PI * DEFAULT_FREQ * dt * static_cast<double>(index)));

#ifdef USE_CONSOLE
            std::cout << p[index] << ", ";
            //if (index % 20 == 0) { std::cout << '\n'; }
#endif
        }

        return p;
    }

    [[noreturn]] void AudioPlayerTest() {
        const auto p = AllocBuffer();

        AudioPlayer player;
        AudioData audioData;
        AudioPlayer::StartAudio(audioData);

        for (;;) {
            audioData.AddData({ p,DEFAULT_BUFFER_SZ,DEFAULT_DURA });
            Sleep(DEFAULT_DURA + rand() % 10);
        }
    }

    [[noreturn]] void AudioHandlerTest() {
        const auto p = AllocBuffer();

        AudioHandler handler = { 4 * DEFAULT_DURA };
        handler.AddData({ p,DEFAULT_BUFFER_SZ,DEFAULT_DURA });

        for (;;) {
            handler.AddData({ p,DEFAULT_BUFFER_SZ,DEFAULT_DURA });
            Sleep(DEFAULT_DURA + rand() % 10);
        }
    }
}