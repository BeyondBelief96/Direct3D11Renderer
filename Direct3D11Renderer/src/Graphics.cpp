#include "Graphics.h"
#include "D3Utils.h"
#include <sstream>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

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


Graphics::Graphics(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = 0;
    swapChainDesc.BufferDesc.Height = 0;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    UINT swapCreateFlags = 0u;
#ifdef _DEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr;

    // Create device and front/back buffers, and swap chain and rendering context.
    GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swapCreateFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        pSwapChain.GetAddressOf(),
        pDevice.GetAddressOf(),
        nullptr,
        pContext.GetAddressOf()
    ));

    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    GFX_THROW_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pTarget.GetAddressOf()));
}

void Graphics::EndFrame()
{
    HRESULT hr;
#ifdef _DEBUG
    infoManager.Set();
#endif
    if (FAILED(hr = pSwapChain->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
#ifdef _DEBUG
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
#else
            throw GFX_DEVICE_REMOVED_EXCEPT(hr);
#endif
        }
        else
        {
            throw GFX_EXCEPT(hr);
        }
    }
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
    const float color[] = { red, green, blue, 1.0f };
    pContext->ClearRenderTargetView(pTarget.Get(), color);
}