#pragma once
#include <Windows.h>
#include <stringapiset.h>
#include <string>

inline std::wstring StringToWString(const std::string& str)
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