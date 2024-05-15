#pragma once

#include <string_view>

bool LoadFileToWStr(std::wstring& wStr, const wchar_t* pFilePath, bool bUnicode = true);