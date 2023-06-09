#pragma once
#include <map>
#include <Windows.h>


BOOL CopyFileToClipboard(HWND hWnd);
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
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OpenButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	// global map init
	static std::map<HWND, Window*> windowMap;
	// creating hwindows instance w32
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	HMENU hMenu;
	HMENU hFileMenu;
	HMENU hEditMenu;
	HMENU hOpenSubMenu;
};

