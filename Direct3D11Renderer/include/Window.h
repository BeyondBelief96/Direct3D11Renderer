#pragma once
#include "ChiliWin.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

class Window
{
public:
	
private:
	// Singleton WindowClass
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass() noexcept;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName = L"D3DEngine";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int width, int height, const WCHAR* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const WCHAR* title);
	static std::optional<int> ProcessMessages();

	void CaptureMouse();
	void ReleaseMouse();
	bool IsMouseCaptured() const noexcept;
	Graphics& Gfx();

	Keyboard kbd;
	Mouse mouse;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	int width;
	int height;
	HWND hwnd;
	bool mouseCaptured = false;
	POINT originalMousePos = {};
	std::unique_ptr<Graphics> pGraphics;
};