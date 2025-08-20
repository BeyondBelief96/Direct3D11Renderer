#pragma once

#include <stdexcept>
#include <wincodec.h>
#include <wrl/client.h>

class WICFactory
{
public:
	static IWICImagingFactory* GetFactory()
	{
		if (!pFactory)
		{
			// Initialize the COM library
			HRESULT hr = CoInitialize(nullptr);
			if (FAILED(hr))
			{
				throw std::runtime_error("Failed to initialize COM library");
			}

			hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
				IID_PPV_ARGS(&pFactory));

			if (FAILED(hr))
			{
				CoUninitialize();
				throw std::runtime_error("Failed to create WIC Imaging Factory");
			}
		}
		return pFactory.Get();
	}

	static void Shutdown()
	{
		pFactory.Reset();
		CoUninitialize();
	}
private:
	static Microsoft::WRL::ComPtr<IWICImagingFactory> pFactory;
};

Microsoft::WRL::ComPtr<IWICImagingFactory> WICFactory::pFactory = nullptr;