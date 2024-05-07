#include "LoadFileToWStr.h"

#include <_DeLib/Encryption.h>
#include <_DeLib/Encryption.cpp>

bool LoadFileToWStr(std::wstring& wStr, const wchar_t* pFilePath, bool bUnicode)
{
    // load file
    Encryption e;
    e.OpenFile(pFilePath);

    // handle unicode
    constexpr auto UTF8_SIGNATURE = "\xEF\xBB\xBF";

    auto pSrc = reinterpret_cast<char*>(e.GetInputData());
    auto len = e.GetInputDataLength();

    // BOM
    if ((len >= 3) && (memcmp(pSrc, UTF8_SIGNATURE, 3) == 0)) {
        bUnicode = true;

    	pSrc += 3;
        len -= 3;
    }

    if (len == 0) {
        return false;
    }

    return to_wide_string(wStr, pSrc, len,
        bUnicode ? CP_UTF8 : CP_ACP);
}
