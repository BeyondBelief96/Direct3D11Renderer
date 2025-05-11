#include "Graphics.h"
#include "D3Utils.h"
#include <sstream>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace DX = DirectX;

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

    // Setup the z-buffer
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
	depthStencilBufferDesc.Width = 800;
	depthStencilBufferDesc.Height = 600;
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
	pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawTestTriangle(float angle, float mouseX, float z)
{
    struct Vertex
    {
        struct
        {
            float x;
            float y;
            float z;
        } pos;
    };

    const Vertex vertices[] =
    {
		// front bottom left
		{ -1.0f, -1.0f, -1.0f},
		// front bottom right
        { 1.0f, -1.0f, -1.0f},
		// front top left
		{ -1.0f, 1.0f, -1.0f},
		// front top right
		{ 1.0f, 1.0f, -1.0f},
		// back bottom left
		{ -1.0f, -1.0f, 1.0f},
		// back bottom right
		{ 1.0f, -1.0f, 1.0f},
		// back top left
		{ -1.0f, 1.0f, 1.0f},
		// back top right
		{ 1.0f, 1.0f, 1.0f},
    };

	// Creating hresult and blob for shader compilation
    HRESULT hr;
    Microsoft::WRL::ComPtr<ID3DBlob> pBlob;

	// Create vertex buffer
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.CPUAccessFlags = 0u;
    bufferDesc.MiscFlags = 0u;
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA bufferData{};
    bufferData.pSysMem = vertices;
    GFX_THROW_INFO(pDevice->CreateBuffer(&bufferDesc, &bufferData, &pVertexBuffer));

    // Bind Vertex buffer to pipeline
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0u;
    pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

	// Create index buffer
    const unsigned short indices[] =
    {
        // Front face
        0, 2, 1,    // Triangle 1: Front Bottom Left, Front Top Left, Front Bottom Right
        2, 3, 1,    // Triangle 2: Front Top Left, Front Top Right, Front Bottom Right

        // Right face
        1, 3, 5,    // Triangle 1: Front Bottom Right, Front Top Right, Back Bottom Right
        3, 7, 5,    // Triangle 2: Front Top Right, Back Top Right, Back Bottom Right

        // Top face
        2, 6, 3,    // Triangle 1: Front Top Left, Back Top Left, Front Top Right
        3, 6, 7,    // Triangle 2: Front Top Right, Back Top Left, Back Top Right

        // Back face
        4, 5, 7,    // Triangle 1: Back Bottom Left, Back Bottom Right, Back Top Right
        4, 7, 6,    // Triangle 2: Back Bottom Left, Back Top Right, Back Top Left

        // Left face
        0, 4, 2,    // Triangle 1: Front Bottom Left, Back Bottom Left, Front Top Left
        2, 4, 6,    // Triangle 2: Front Top Left, Back Bottom Left, Back Top Left

        // Bottom face
        0, 1, 4,    // Triangle 1: Front Bottom Left, Front Bottom Right, Back Bottom Left
        1, 5, 4,    // Triangle 2: Front Bottom Right, Back Bottom Right, Back Bottom Left
    };

	Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.CPUAccessFlags = 0u;
	indexBufferDesc.MiscFlags = 0u;
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.StructureByteStride = sizeof(unsigned short);
	D3D11_SUBRESOURCE_DATA indexBufferData{};
	indexBufferData.pSysMem = indices;
	GFX_THROW_INFO(pDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &pIndexBuffer));

	// Bind index buffer to pipeline
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	// Setup input layout
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

    // Create a constant buffer for transformation matrix
    struct ConstantBuffer
    {
        DX::XMMATRIX transform;
    };

	// Initialize the transformation matrix to a z-rotation matrix
    const ConstantBuffer cb
    {
        {
            DX::XMMatrixTranspose(
                DX::XMMatrixRotationZ(angle) * 
                DX::XMMatrixRotationX(angle) *
                DX::XMMatrixTranslation(mouseX, 0.0f, z + 4.0f) *
                DX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f))
        }
    };

	// Create constant buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC constantBufferDesc{};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0u;
	constantBufferDesc.ByteWidth = sizeof(cb);
	constantBufferDesc.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA constantBufferData{};
	constantBufferData.pSysMem = &cb;
	GFX_THROW_INFO(pDevice->CreateBuffer(&constantBufferDesc, &constantBufferData, pConstantBuffer.GetAddressOf()));

	// Bind constant buffer to vertex shader
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	// Set primitive topology
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Constant Buffer for face colors
    struct ConstantBuffer2
    {
        struct
        {
            float r; 
            float g;
			float b;
            float a;
        } face_color[6];
    };

	// Initialize the face colors
    const ConstantBuffer2 cb2
    {
        {
            {1.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 1.0f, 1.0f}
        }
    };

	// Create constant buffer for face colors
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer2;
	D3D11_BUFFER_DESC constantBufferDesc2{};
	constantBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc2.MiscFlags = 0u;
	constantBufferDesc2.ByteWidth = sizeof(cb2);
	constantBufferDesc2.StructureByteStride = 0u;
	D3D11_SUBRESOURCE_DATA constantBufferData2{};
	constantBufferData2.pSysMem = &cb2;
	GFX_THROW_INFO(pDevice->CreateBuffer(&constantBufferDesc2, &constantBufferData2, pConstantBuffer2.GetAddressOf()));

    pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());

	// Create pixel shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	// Bind pixel shader to pipeline
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

    // Create vertex shader
    Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

    GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

    //Bind vertex shader to pipeline
    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	// Create input layout (Rearranged load order of shaders since we need the vertex shader to create the input layout)
    GFX_THROW_INFO(pDevice->CreateInputLayout(
        layout,
        (UINT)std::size(layout),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout));

	// Bind input layout to pipeline
	pContext->IASetInputLayout(pInputLayout.Get());

    // Configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = 800.0f;
    vp.Height = 600.0f;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
    pContext->RSSetViewports(1u, &vp);

    GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u, 0u));
}
