#include "Exceptions/GraphicsExceptions.h"
#include <sstream>

const char* GraphicsHrException::GetType() const noexcept
{
    return "Graphics Exception";
}

const char* DeviceRemovedException::GetType() const noexcept
{
    return "Graphics Device Removed Exception";
}

InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
    : D3Exception(line, file)
{
    // Join all info messages with newlines into single string
    for (const auto& m : infoMsgs)
    {
        info += m;
        info += "\n";
    }
    // Remove final newline if exists
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* InfoException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "\n[Error Info]\n" << GetErrorInfo() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* InfoException::GetType() const noexcept
{
    return "Graphics Info Exception";
}

std::string InfoException::GetErrorInfo() const noexcept
{
    return info;
}