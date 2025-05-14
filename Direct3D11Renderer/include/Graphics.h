#pragma once
#include "ChiliWin.h"
#include "Exceptions/GraphicsExceptions.h" 
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include <DirectXMath.h>
#ifdef _DEBUG
#include "DxgiInfoManager.h"
#endif

class Graphics
{
public:
    Graphics(HWND hwnd);
    ~Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics& rhs) = delete;

    void EndFrame();
    void ClearBuffer(float red, float green, float blue) noexcept;
    void DrawIndexed(UINT count) noexcept(!_DEBUG);
    DirectX::XMMATRIX GetProjection() const noexcept;
    void SetProjection(DirectX::FXMMATRIX proj) noexcept;

	ID3D11DeviceContext* const GetContext() noexcept;
    ID3D11Device* const GetDevice() noexcept;
#ifdef _DEBUG
	DxgiInfoManager& GetInfoManager() noexcept;
#endif

private:
    DirectX::XMMATRIX projection;
#ifdef _DEBUG
    DxgiInfoManager infoManager;
#endif
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};

