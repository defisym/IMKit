#include "LoadFileToWStr.h"

#include <_DeLib/GeneralDefinition.h>

#include <_DeLib/Encryption.h>
#include <_DeLib/Encryption.cpp>

bool LoadFileToWStr(std::wstring& wStr, const wchar_t* pFilePath, bool bUnicode) {
    // load file
    Encryption e;
    e.OpenFile(pFilePath);

	const auto pSrc = reinterpret_cast<char*>(e.GetInputData());
	const auto len = e.GetInputDataLength();

    return LoadData(wStr, pSrc, len, bUnicode);
}
