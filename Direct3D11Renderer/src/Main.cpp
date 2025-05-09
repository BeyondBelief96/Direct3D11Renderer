#include "Application.h"
#include "D3Utilities.h"
#include <sstream>


int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        Application app{};
        return app.Run();
    }
    catch (const D3Exception& e)
    {
        std::wstring whatMsg = StringToWString(e.what());
        std::wstring typeMsg = StringToWString(e.GetType());
        MessageBox(nullptr, whatMsg.c_str(), typeMsg.c_str(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e)
    {
        std::wstring whatMsg = StringToWString(e.what());
        MessageBox(nullptr, whatMsg.c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return -1;
	
}