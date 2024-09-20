//#define USE_CONSOLE

#ifdef USE_CONSOLE
#pragma warning(disable : 4996)
#include <iostream>
#endif

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include "./../../Src/DLL/include/OTDR.h"
#include "./../Utilities/AudioPlayer.h"

namespace AudioTest {
    const OTDRProcessValueType* AllocBuffer(const size_t sz = 1000) {
#ifdef USE_CONSOLE
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
#endif

        const auto p = new OTDRProcessValueType[sz];
        for (size_t index = 0; index < sz; index++) {
            p[index] = static_cast<OTDRProcessValueType>(std::sin(PI * index / 200.0));

#ifdef USE_CONSOLE
            std::cout << p[index] << ", ";
            if (index % 20 == 0) { std::cout << '\n'; }
#endif
        }

        return p;
    }

    [[noreturn]] void AudioPlayerTest() {
        const auto p = AllocBuffer();

        AudioPlayer player;
        AudioData audioData;
        AudioPlayer::StartAudio(audioData);
        Sleep(500 + rand() % 10);

        for (;;) {
            player.AddData(audioData, { p,1000,500 });
            Sleep(500 + rand() % 10);
        }
    }

    [[noreturn]] void AudioHandlerTest() {
        const auto p = AllocBuffer();

        AudioHandler handler = { 500 };
        handler.AddData({ p,1000,500 });

        //Sleep(500 + rand() % 10);

        for (;;) {
            //handler.AddData({ p,1000,500 });
            //Sleep(500 + rand() % 10);
        }
    }
}