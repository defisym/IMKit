#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "./../../Src/DLL/include/OTDR.h"
#include "./../Utilities/AudioPlayer.h"

namespace AudioPlayerTest {
    [[noreturn]] void AudioPlayerTest() {
        auto p = new OTDRProcessValueType[1000];
        for (size_t index = 0; index < 1000; index++) {
            p[index] = std::sin(PI * index / 200.0);
        }

        AudioPlayer player;
        AudioData audioData;
        player.StartAudio(audioData);
        Sleep(500 + rand() % 10);

        for (;;) {
            player.AddData(audioData, { p ,1000,500 });
            Sleep(500 + rand() % 10);
        }
    }
}