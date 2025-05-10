#include "GraphicsExceptions.h"
#include "Window.h"
#include <sstream>

HrException::HrException(int line, const char* file, HRESULT hr, const char* functionCall) noexcept
    : GraphicsException(line, file),
    hr(hr),
    functionCall(functionCall ? functionCall : "")
{
}

HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs, const char* functionCall) noexcept
    : GraphicsException(line, file),
    hr(hr),
    functionCall(functionCall ? functionCall : "")
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

const char* HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;

    if (!info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl;
    }

    if (!functionCall.empty())
    {
        oss << "[Function Call] " << GetFunctionCall() << std::endl;
    }

    oss << GetOriginString();

    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* HrException::GetType() const noexcept
{
    return "D3DEngine Graphics Exception";
}

HRESULT HrException::GetErrorCode() const noexcept
{
    return hr;
}

std::string HrException::GetErrorString() const noexcept
{
    std::ostringstream oss;
    oss << "HRESULT: 0x" << std::hex << std::uppercase << hr;
    return oss.str();
}

std::string HrException::GetErrorDescription() const noexcept
{
    return Window::Exception::TranslateErrorCode(hr);
}

std::string HrException::GetFunctionCall() const noexcept
{
    return functionCall;
}

std::string HrException::GetErrorInfo() const noexcept
{
    return info;
}

const char* DeviceRemovedException::GetType() const noexcept
{
    return "D3DEngine Graphics Device Removed Exception";
}

InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
    : GraphicsException(line, file)
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
    return "D3DEngine Graphics Info Exception";
}

std::string InfoException::GetErrorInfo() const noexcept
{
    return info;
}