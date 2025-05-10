#pragma once
#include "ChiliWin.h"
#include "GraphicsExceptions.h" 
#include <d3d11.h>
#include <vector>
#include <wrl.h>

#ifdef _DEBUG
#include "DxgiInfoManager.h"
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

private:
#ifdef _DEBUG
    DxgiInfoManager infoManager;
#endif
    Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
    Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};