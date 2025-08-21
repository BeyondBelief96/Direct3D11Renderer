#pragma once
#include "D3Exception.h"
#include "Utilities/ChiliWin.h"
#include <vector>

class HrHelper
{
public:
    // Static helper for error code translation
    static std::string TranslateErrorCode(HRESULT hr) noexcept;
};

// Base class for all HRESULT-related exceptions
class HrException : public D3Exception
{
public:
    HrException(int line, const char* file, HRESULT hr, const char* functionCall = "") noexcept;
    HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs, const char* functionCall = "") noexcept;

    const char* what() const noexcept override;
    HRESULT GetErrorCode() const noexcept;
    std::string GetErrorString() const noexcept;
    std::string GetErrorDescription() const noexcept;
    std::string GetFunctionCall() const noexcept;
    std::string GetErrorInfo() const noexcept;

private:
    HRESULT hr;
    std::string functionCall;
    std::string info;
};