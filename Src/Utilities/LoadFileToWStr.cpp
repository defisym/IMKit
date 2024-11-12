#include "LoadFileToWStr.h"

#include <GeneralDefinition.h>

#include <Encryption.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <Encryption.cpp>

bool LoadFileToWStr(std::wstring& wStr, const wchar_t* pFilePath, bool bUnicode) {
    // load file
    Encryption e;
    e.OpenFile(pFilePath);

	const auto pSrc = reinterpret_cast<char*>(e.GetInputData());
	const auto len = e.GetInputDataLength();

    return LoadData(wStr, pSrc, len, bUnicode);
}
