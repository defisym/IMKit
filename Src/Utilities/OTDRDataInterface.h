#pragma once

#include <_DeLib/CSVHandler.h>

#include "../Src/DLL/include/OTDR.h"

struct OTDRData {
    const OTDRProcessValueType* pData = nullptr;
    size_t sz = 0u;
};

struct OTDRDataStringify {
    CSV::CSVBuilder<std::string> builder;

    [[nodiscard]] const std::string& ToString(const OTDRData& data, const bool bBinary = false);
};

