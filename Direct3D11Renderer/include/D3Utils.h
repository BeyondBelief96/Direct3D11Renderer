#pragma once
#include "ChiliWin.h"
#include <string>

class D3Utils
{
public:
	static std::string WstringToNarrow(const std::wstring& wideStr) noexcept;
	static std::string WcharToNarrow(const WCHAR* wideStr) noexcept;
	static std::wstring StringToWString(const std::string& str) noexcept;
};
