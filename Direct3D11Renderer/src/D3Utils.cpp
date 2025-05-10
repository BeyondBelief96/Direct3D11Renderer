#include "D3Utils.h"

std::string D3Utils::WstringToNarrow(const std::wstring& wideStr) noexcept
{
	if (wideStr.empty())
	{
		return std::string();
	}

	std::string narrowStr;
	int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (requiredSize > 0)
	{
		narrowStr.resize(requiredSize);
		WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &narrowStr[0], requiredSize, nullptr, nullptr);
		narrowStr.resize(requiredSize - 1); // Remove the null terminator
	}

	return narrowStr;
}

std::string D3Utils::WcharToNarrow(const WCHAR* wideStr) noexcept
{
	if (wideStr == nullptr) return std::string();

	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
	if (size_needed <= 0) return std::string();

	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &strTo[0], size_needed, nullptr, nullptr);
	strTo.resize(size_needed - 1); // Remove null terminator
	return strTo;
}

std::wstring D3Utils::StringToWString(const std::string& str) noexcept
{
	if (str.empty())
		return std::wstring();

	// Get the required size for the buffer
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), nullptr, 0);
	std::wstring wstrTo(size_needed, 0);
	// Perform the actual conversion
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

