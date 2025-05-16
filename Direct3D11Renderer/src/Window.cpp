#pragma once
#include "Window.h"
#include "Exceptions/WindowExceptions.h"
#include <sstream>
#include "resource.h"
#include "imgui/imgui_impl_win32.h"

// Forward declaration of the ImGui_ImplWin32_WndProcHandler function
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// WindowClass
Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() noexcept
{
	UnregisterClass(GetName(), GetInstance());
}


const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

// Window
Window::Window(int width, int height, const WCHAR* name)
	: width(width), height(height), hwnd(nullptr)
{
	RECT wr;
	wr.left = 0;
	wr.right = width + wr.left;
	wr.top = 0;
	wr.bottom = height + wr.top;

	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw WND_LAST_EXCEPT();
	};

	hwnd = CreateWindow(
		WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		WindowClass::GetInstance(),
		this
	);

	if (hwnd == nullptr)
	{
		throw WND_LAST_EXCEPT();
	}

	ShowWindow(hwnd, SW_SHOW);

	// Initialize ImGui
	ImGui_ImplWin32_Init(hwnd);

	// Create graphics object
	pGraphics = std::make_unique<Graphics>(hwnd, width, height);
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hwnd);
}

void Window::SetTitle(const WCHAR* title)
{
	if (SetWindowText(hwnd, title) == 0)
	{
		throw WND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return std::optional<int>(static_cast<int>(msg.wParam));
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return std::optional<int> {};
}

void Window::CaptureMouse()
{
	if (!mouseCaptured)
	{
		// Store original mouse position
		GetCursorPos(&originalMousePos);

		// Hide Cursor
		ShowCursor(FALSE);

		// Confine cursor to window
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		POINT upperLeft = { 0,0 };
		ClientToScreen(hwnd, &upperLeft);

		POINT lowerRight = { clientRect.right, clientRect.bottom };

		RECT clipRect = { upperLeft.x, upperLeft.y, lowerRight.x, lowerRight.y };
		ClipCursor(&clipRect);

		// Center cursor in window
		POINT centerPos = { width / 2, height / 2 };
		ClientToScreen(hwnd, &centerPos);
		SetCursorPos(centerPos.x, centerPos.y);

		mouseCaptured = true;

		SetCapture(hwnd);
	}
}

void Window::ReleaseMouse()
{
	if (mouseCaptured)
	{
		// Show Cursor
		ShowCursor(TRUE);
		// Release cursor from window
		ClipCursor(nullptr);
		// Restore original mouse position
		SetCursorPos(originalMousePos.x, originalMousePos.y);

		// Release capture
		ReleaseCapture();

		mouseCaptured = false;
	}
}

bool Window::IsMouseCaptured() const noexcept
{
	return mouseCaptured;
}

Graphics& Window::Gfx()
{
	if (!pGraphics)
	{
		throw WND_NOGFX_EXCEPT();
	}

	return *pGraphics;
}

LRESULT Window::HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set message procedure to normal (non-setup) handler now that setup is finished
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// forward the message to the window class
		return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
	}

	// all other messages are forwarded to the default window procedure
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// retrieve pointer to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	// forward the message to the window class
	return pWnd->HandleMsg(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
		/*********** KEYBOARD MESSAGES ***********/
	case WM_KEYDOWN:
		// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	case WM_SYSKEYDOWN:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // filter autorepeat
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		if (ImGui::GetIO().WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/*********** END KEYBOARD MESSAGES ***********/

		/************* MOUSE MESSAGES ****************/
	case WM_MOUSEMOVE:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}

		const POINTS pt = MAKEPOINTS(lParam);

		if (mouseCaptured)
		{
			// When mouse is captured, calculate relative movement from center
			int deltaX = pt.x - width / 2;
			int deltaY = pt.y - height / 2;

			// Only process if there's actual movement
			if (deltaX != 0 || deltaY != 0)
			{
				// Notify mouse of movement using deltas
				mouse.OnMouseMove(deltaX, deltaY, true);

				// Reset cursor to center
				POINT centerPos = { width / 2, height / 2 };
				ClientToScreen(hwnd, &centerPos);
				SetCursorPos(centerPos.x, centerPos.y);
			}
		}
		else
		{
			// Normal mouse movement
			mouse.OnMouseMove(pt.x, pt.y, false);

			// When mouse enters the window, set capture and notify
			if (!mouse.IsInWindow())
			{
				SetCapture(hwnd);
				mouse.OnMouseEnter();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (ImGui::GetIO().WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	/************** END MOUSE MESSAGES **************/
	}


	return DefWindowProc(hwnd, msg, wParam, lParam);
}