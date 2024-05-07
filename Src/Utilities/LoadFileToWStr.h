#pragma once
#include <string_view>

bool LoadFileToWStr(std::wstring& result, const wchar_t* pFilePath, bool bUnicode = true);