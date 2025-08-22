#pragma once
#include "Exceptions/HrException.h"

// Graphics-specific HRESULT exception
class GraphicsHrException : public HrException
{
public:
    using HrException::HrException;
    const char* GetType() const noexcept override;
};

// Exception for device removed errors
class DeviceRemovedException : public GraphicsHrException
{
public:
    using GraphicsHrException::GraphicsHrException;
    const char* GetType() const noexcept override;
};

// Exception for info-only messages from DXGI debug layer
class DxgiDebugException : public D3Exception
{
public:
    DxgiDebugException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
    const char* what() const noexcept override;
    const char* GetType() const noexcept override;
    std::string GetErrorInfo() const noexcept;
private:
    std::string info;
};

// Graphics exception checking/throwing macros
// HRESULT hr should exist in the local scope for these macros to work
#define GFX_EXCEPT_NOINFO(hr) GraphicsHrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw GFX_EXCEPT_NOINFO(hr)

#ifdef _DEBUG
#define GFX_EXCEPT(hr) GraphicsHrException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); call; {auto v = infoManager.GetMessages(); if(!v.empty()) {throw DxgiDebugException(__LINE__, __FILE__, v);}}
#else
#define GFX_EXCEPT(hr) GraphicsHrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) call
#endif

// macro for importing infomanager into local scope
// this.GetInfoManager(Graphics& gfx) must exist
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else
#define DEBUGMANAGER(gfx) HRESULT hr; DxgiDebugManager& infoManager = GetInfoManager((gfx))
#endif