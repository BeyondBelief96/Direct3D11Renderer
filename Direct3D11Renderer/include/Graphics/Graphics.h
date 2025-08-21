#pragma once
#include "Utilities/ChiliWin.h"
#include "Exceptions/GraphicsExceptions.h" 
#include <d3d11.h>
#include <vector>
#include <wrl.h>
#include <DirectXMath.h>
#ifdef _DEBUG
#include "Exceptions/DxgiDebugManager.h"
#endif

class Graphics
{
public:
    Graphics(HWND hwnd, int width, int height);
    ~Graphics();
    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics& rhs) = delete;

    void BeginFrame(float red, float green, float blue);
    void EndFrame();
    void DrawIndexed(UINT count) noexcept(!_DEBUG);

    DirectX::XMMATRIX GetProjection() const noexcept;
    void SetProjection(DirectX::FXMMATRIX proj) noexcept;

	DirectX::XMMATRIX GetView() const noexcept;
	void SetView(DirectX::FXMMATRIX cam) noexcept;

    DirectX::XMMATRIX GetViewProjection() const noexcept;

    void EnableImgui() noexcept;
	void DisableImgui() noexcept;
    bool IsImguiEnabled() const noexcept;

	ID3D11DeviceContext* const GetContext() noexcept;
    ID3D11Device* const GetDevice() noexcept;
#ifdef _DEBUG
	DxgiDebugManager& GetInfoManager() noexcept;
#endif

private:
    bool imguiEnabled = true;
    int viewportWidth;
	int viewportHeight;
    DirectX::XMMATRIX projection;
    DirectX::XMMATRIX view;
#ifdef _DEBUG
    DxgiDebugManager infoManager;
#endif
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;
};

