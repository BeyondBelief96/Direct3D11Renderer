#pragma once
#include "Exceptions/HrException.h"

// Window-specific HRESULT exception
class WindowHrException : public HrException
{
public:
    using HrException::HrException;
    const char* GetType() const noexcept override;
};

// Exception for when graphics object is missing
class WindowNoGfxException : public D3Exception
{
public:
    using D3Exception::D3Exception;
    const char* GetType() const noexcept override;
};

// Window error handling macros
#define WND_EXCEPT(hr) WindowHrException(__LINE__, __FILE__, (hr))
#define WND_LAST_EXCEPT() WindowHrException(__LINE__, __FILE__, GetLastError())
#define WND_NOGFX_EXCEPT() WindowNoGfxException(__LINE__, __FILE__)