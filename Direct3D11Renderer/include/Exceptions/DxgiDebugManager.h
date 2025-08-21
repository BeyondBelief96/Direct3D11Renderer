#pragma once
#include "Utilities/ChiliWin.h"
#include <wrl.h>
#include <vector>
#include <string>
#include <dxgidebug.h>

class DxgiDebugManager
{
public:
    DxgiDebugManager();
    ~DxgiDebugManager() = default;
    DxgiDebugManager(const DxgiDebugManager&) = delete;
    DxgiDebugManager& operator=(const DxgiDebugManager&) = delete;

    void Set() noexcept;
    std::vector<std::string> GetMessages() const;

private:
    unsigned long long next = 0u;
    Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};