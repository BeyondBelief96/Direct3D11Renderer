#include <Windows.h>
#include <string>
#include "Window.h"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
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