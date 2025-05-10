#pragma once
#include "ChiliWin.h"
#include "D3Exception.h"
#include <string>
#include <vector>

// Forward declarations
class Graphics;

class GraphicsException : public D3Exception
{
    using D3Exception::D3Exception;
};

class HrException : public GraphicsException
{
public:
    HrException(int line, const char* file, HRESULT hr, const char* functionCall = "") noexcept;
    HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs, const char* functionCall = "") noexcept;
    const char* what() const noexcept override;
    const char* GetType() const noexcept override;
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

class DeviceRemovedException : public HrException
{
    using HrException::HrException;
public:
    const char* GetType() const noexcept override;
private:
    std::string reason;
};

class InfoException : public GraphicsException
{
public:
    InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
    const char* what() const noexcept override;
    const char* GetType() const noexcept override;
    std::string GetErrorInfo() const noexcept;
private:
    std::string info;
};