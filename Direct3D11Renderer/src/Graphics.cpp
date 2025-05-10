#include "Graphics.h"
#include "D3Utils.h"
#include <sstream>
#pragma comment(lib, "d3d11.lib")

// graphics exception checking/throwing macros 
#define GFX_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hr)
#define GFX_THROW_REMOVED_EXCEPT(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::DeviceRemovedException(__LINE__,__FILE__,hr)


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
	swapChainDesc.OutputWindow = (HWND)69;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr;

	// Create device and front/back buffers, and swap chain and rendering context.
	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&pSwapChain,
		&pDevice,
		nullptr,
		&pContext
	));

	ID3D11Resource* pBackBuffer = nullptr;
	GFX_THROW_FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
	GFX_THROW_FAILED(pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget));
	pBackBuffer->Release();
}

Graphics::~Graphics()
{
	if(pTarget)
	{
		pTarget->Release();
	}
	if (pContext)
	{
		pContext->Release();
	}
	if (pSwapChain)
	{
		pSwapChain->Release();
	}
	if (pDevice)
	{
		pDevice->Release();
	}
}

void Graphics::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			GFX_THROW_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			GFX_THROW_FAILED(hr);
		}
	}
	pSwapChain->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	const float color[] = { red, green, blue, 1.0f };
	pContext->ClearRenderTargetView(pTarget, color);
}

/* Exception Handling */
Graphics::HrException::HrException(int line, const char* file, HRESULT hr) noexcept
	:
	D3Exception(line, file),hr(hr)
{
}

const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x " << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString() << std::endl;

	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "D3DEngine Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	std::ostringstream oss;
	oss << "HRESULT: 0x" << std::hex << std::uppercase << hr;
	return oss.str();
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
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

	return D3Utils::WstringToNarrow(wideErrorString);
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "D3DEngine Graphics Device Removed Exception";
}
