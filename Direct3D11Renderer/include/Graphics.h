#pragma once
#include "ChiliWin.h"
#include "GraphicsExceptions.h" 
#include <d3d11.h>
#include <vector>
#include <wrl.h>

#ifdef _DEBUG
#include "DxgiInfoManager.h"
#endif

// Graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw GFX_EXCEPT_NOINFO(hr)

#ifdef _DEBUG
#define GFX_EXCEPT(hr) HrException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); call; {auto v = infoManager.GetMessages(); if(!v.empty()) {throw InfoException(__LINE__, __FILE__, v);}}
#else
#define GFX_EXCEPT(hr) HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) call
#endif

class Graphics
{
public:
    Graphics(HWND hwnd);
    ~Graphics() = default;
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics& rhs) = delete;


    void EndFrame();
    void ClearBuffer(float red, float green, float blue) noexcept;
    void DrawTestTriangle();

private:
#ifdef _DEBUG
    DxgiInfoManager infoManager;
#endif
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};

