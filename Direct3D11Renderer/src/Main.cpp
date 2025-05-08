#include <Windows.h>
#include <string>
#include "Window.h"
#include "D3Exception.h"

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
        }

        if (gResult == -1)
        {
            return -1;
        }

        return static_cast<int>(msg.wParam);
    }
    catch (const D3Exception& e)
    {
		MessageBox(nullptr, (LPCWSTR)e.what(), (LPCWSTR)e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e)
    {
		MessageBox(nullptr, (LPCWSTR)e.what(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"Unknown Exception", L"Error", MB_OK | MB_ICONEXCLAMATION);
    }
	
}