#include "Window.h"
#include "D3Utilities.h"


int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    try
    {
        Window wnd(800, 600, L"D3DEngine");

        MSG msg;
        BOOL gResult;
        while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (wnd.kbd.KeyIsPressed(VK_MENU))
            {
				MessageBox(nullptr, L"Space key pressed", L"Key Pressed", MB_OK);
            }
        }

        if (gResult == -1)
        {
            return -1;
        }

        return static_cast<int>(msg.wParam);
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