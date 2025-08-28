#include "Graphics/Graphics.h"
#include "Utilities/D3Utils.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace DX = DirectX;

Graphics::Graphics(HWND hwnd, int width, int height) 
    : projection(DX::XMMatrixIdentity()),
    view(DX::XMMatrixIdentity()),
	viewportWidth(width),
	viewportHeight(height)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
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

    // Gain access to texture subresource in swap chain (back buffer)
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    GFX_THROW_INFO(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, pTarget.GetAddressOf()));

    // Setup the depth/stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&depthStencilDesc, pDepthStencilState.ReleaseAndGetAddressOf()));

	// Bind the depth stencil state to the pipeline
	pContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);

    // create depth stencil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencilBuffer;
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};
	depthStencilBufferDesc.Width = viewportWidth;
	depthStencilBufferDesc.Height = viewportHeight;
	depthStencilBufferDesc.MipLevels = 1u;
	depthStencilBufferDesc.ArraySize = 1u;
    depthStencilBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilBufferDesc.SampleDesc.Count = 1u;
	depthStencilBufferDesc.SampleDesc.Quality = 0u;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, pDepthStencilBuffer.GetAddressOf()));

	// create depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0u;
    GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencilBuffer.Get(), &depthStencilViewDesc, pDepthStencilView.GetAddressOf()));

	// Bind the render target and depth stencil view to the pipeline
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepthStencilView.Get());

    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = (float)viewportWidth;
    vp.Height = (float)viewportHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    pContext->RSSetViewports(1u, &vp);

	// Intialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());
}

Graphics::~Graphics()
{
    ImGui_ImplWin32_Shutdown();
    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();
}

void Graphics::BeginFrame(float red, float green, float blue)
{
	if (imguiEnabled)
	{
		ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
	}

    const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget.Get(), color);
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::EndFrame()
{
    if (imguiEnabled)
    {
        ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

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

void Graphics::DrawIndexed(UINT count) noexcept(!_DEBUG)
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

DX::XMMATRIX Graphics::GetProjection() const noexcept
{
    return projection;
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX Graphics::GetView() const noexcept
{
    return view;
}

void Graphics::SetView(DirectX::FXMMATRIX cam) noexcept
{
    view = cam;
}

DirectX::XMMATRIX Graphics::GetViewProjection() const noexcept
{
    return view * projection;
}

void Graphics::EnableImgui() noexcept
{
    imguiEnabled = true;
}

void Graphics::DisableImgui() noexcept
{
    imguiEnabled = false;
}

bool Graphics::IsImguiEnabled() const noexcept
{
    return imguiEnabled;
}

ID3D11DeviceContext* const Graphics::GetContext() noexcept
{
    return pContext.Get();
}

ID3D11Device* const Graphics::GetDevice() noexcept
{
    return pDevice.Get();
}

#ifdef _DEBUG
DxgiDebugManager& Graphics::GetInfoManager() noexcept
{
    return infoManager;
}
#endif
