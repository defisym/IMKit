#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

#include "Utilities/Logger.h"
#include "Utilities/OTDRLogData.h"

namespace LoggerTest {
    constexpr auto DEFAULT_POINT = 128000;
    constexpr auto DEFAULT_FRAME = 2560;
    constexpr auto DEFAULT_INTERVAL = 1000;

    [[noreturn]] void LoggerTest() {
        const auto pBuffer = new OTDRProcessValueType[DEFAULT_POINT];
        memset(pBuffer, 0, DEFAULT_POINT * sizeof(OTDRProcessValueType));

        constexpr auto conf = LoggerConfig{ 3000,"log/test" };
        auto logger = Logger{ conf };

        auto inf = OTDRLogData{ {false,true} };
        [[maybe_unused]] auto sz = inf.GetEsitimateSize(250'000'000, 2000, 1280, 1000);

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
