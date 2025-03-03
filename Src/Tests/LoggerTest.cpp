#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include "Utilities/Logger.h"
#include "Utilities/OTDRDataInterface.h"
#include "Utilities/VibrationDataInterface.h"
#include "GUIContext/Handler/LoggerHandler.h"

namespace LoggerTest {
    constexpr auto DEFAULT_POINT = 128000;
    constexpr auto DEFAULT_FRAME = 2560;
    constexpr auto DEFAULT_INTERVAL = 1000;

    [[noreturn]] void LoggerTest() {
        const auto pBuffer = new OTDRProcessValueType[DEFAULT_POINT];
        memset(pBuffer, 0, DEFAULT_POINT * sizeof(OTDRProcessValueType));

        constexpr auto conf = FileLoggerConfig{ 3000,"log/test" };
        auto logger = FileLogger{ conf };

        auto inf = VibrationDataInterface{ {false,true} };
        [[maybe_unused]] auto sz = LoggerHandler{}.GetEsitimateSize(250'000'000, 2000, 1280, 1000);

        for (;;) {
            for(size_t index = 0; index < DEFAULT_POINT; index++) {
                pBuffer[index] = static_cast<OTDRProcessValueType>(rand());
            }

            inf.UpdateData({ pBuffer,DEFAULT_POINT });
            logger.AddData(&inf);
            Sleep(DEFAULT_INTERVAL);
        }
    }
}
