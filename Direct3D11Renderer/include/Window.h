#pragma once
#include "ChiliWin.h"
#include "D3Exception.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <optional>

class Window
{
public:
	class Exception : public D3Exception
	{
	public:
		Exception(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		static std::string GetErrorString(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
	private:
		HRESULT hr;
	};
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

	Keyboard kbd;
	Mouse mouse;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	int width;
	int height;
	HWND hwnd;
};

// Error handling macros
#define WND_EXCEPT(hr) Window::Exception(__LINE__, __FILE__, hr)
#define WND_LAST_EXCEPT() Window::Exception(__LINE__, __FILE__, GetLastError())