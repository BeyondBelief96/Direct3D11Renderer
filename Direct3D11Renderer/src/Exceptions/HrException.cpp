#include "Exceptions/HrException.h"
#include <sstream>

std::string HrHelper::TranslateErrorCode(HRESULT hr) noexcept
{
    wchar_t* pMsgBuf = nullptr;
    DWORD nMsgLen = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        hr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&pMsgBuf),
        0, nullptr);

    if (nMsgLen == 0)
    {
        return "Unidentified error code";
    }

    std::wstring wideErrorString = pMsgBuf;
    LocalFree(pMsgBuf);

    std::string narrowStr;
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wideErrorString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (requiredSize > 0)
    {
        narrowStr.resize(requiredSize);
        WideCharToMultiByte(CP_UTF8, 0, wideErrorString.c_str(), -1, &narrowStr[0], requiredSize, nullptr, nullptr);
        narrowStr.resize(requiredSize - 1);
    }

    return narrowStr;
}

HrException::HrException(int line, const char* file, HRESULT hr, const char* functionCall) noexcept
    : D3Exception(line, file),
    hr(hr),
    functionCall(functionCall ? functionCall : "")
{
}

HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs, const char* functionCall) noexcept
    : D3Exception(line, file),
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
    return HrHelper::TranslateErrorCode(hr);
}

std::string HrException::GetFunctionCall() const noexcept
{
    return functionCall;
}

std::string HrException::GetErrorInfo() const noexcept
{
    return info;
}