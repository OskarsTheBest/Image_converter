#pragma once

#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window
{
public:
	Window();
	Window(const Window&) = delete;
	Window& operator =(const Window&) = delete;
	~Window();

	bool ProcessMessages();
	void CreateMenuBar();
	void AddControls();
private:
	// creating hwindows instance w32
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HMENU hMenu;
	HMENU hFileMenu;
	HMENU hEditMenu;
	HMENU hOpenSubMenu;
};

