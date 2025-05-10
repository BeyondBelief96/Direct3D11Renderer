#include "DxgiInfoManager.h"
#include "GraphicsExceptions.h"
#include "D3Utils.h"
#include <memory>
#include <dxgidebug.h>

#pragma comment(lib, "dxguid.lib")

DxgiInfoManager::DxgiInfoManager()
{
    // Define function signature of DXGIGetDebugInterface
    typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

    // Load the DLL that contains the function DXGIGetDebugInterface
    const auto hModDxgiDebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hModDxgiDebug == nullptr)
    {
        throw HrException(__LINE__, __FILE__, HRESULT_FROM_WIN32(GetLastError()));
    }

    // Get address of DXGIGetDebugInterface in dll
    const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
        reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
        );
    if (DxgiGetDebugInterface == nullptr)
    {
        throw HrException(__LINE__, __FILE__, HRESULT_FROM_WIN32(GetLastError()));
    }

    HRESULT hr;
    hr = DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue);
    if (FAILED(hr))
    {
        throw HrException(__LINE__, __FILE__, hr);
    }
}

void DxgiInfoManager::Set() noexcept
{
    // Set the index of the next message to retrieve
    next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::GetMessages() const
{
    std::vector<std::string> messages;
    const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);

    for (auto i = next; i < end; i++)
    {
        SIZE_T messageLength;
        // Get the size of the message
        HRESULT hr = pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
        if (FAILED(hr))
        {
            throw HrException(__LINE__, __FILE__, hr);
        }

        // Allocate memory for the message
        auto bytes = std::make_unique<byte[]>(messageLength);
        auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

        // Get the message
        hr = pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
        if (FAILED(hr))
        {
            throw HrException(__LINE__, __FILE__, hr);
        }

        // Add message to vector
        messages.emplace_back(pMessage->pDescription);
    }

    return messages;
}