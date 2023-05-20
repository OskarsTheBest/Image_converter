#include "Window.h"
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:

		switch (wParam)
		{
		case 1:
			MessageBeep(MB_ICONINFORMATION);
			break;
		case 2:

		case 4:
			DestroyWindow(hWnd);
			break;
		}
		

		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
Window::Window()
	: m_hInstance(GetModuleHandle(nullptr))
{
	const wchar_t* CLASS_NAME = L"IMAGE WINDOW CLASS";

	WNDCLASS wndClass = {};
	wndClass.lpszClassName = CLASS_NAME;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpfnWndProc = WindowProc;

	RegisterClass(&wndClass);

	DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	int width = 640;
	int height = 480;

	// window dimensions
	RECT rect;
	rect.left = 250; // where the box on screen appears
	rect.top = 250;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	AdjustWindowRect(&rect, style, false);

	m_hWnd = CreateWindowEx(
		0,
		L"IMAGE WINDOW CLASS",
		L"Title",
		style,
		rect.left,
		rect.top,
		rect.right - rect.left, // to get the adjusted height
		rect.bottom -rect.top,
		NULL,
		NULL, // can be used for menus
		m_hInstance,
		NULL
	);
	CreateMenuBar();
	AddControls();
	ShowWindow(m_hWnd, SW_SHOW);
}

Window::~Window()
{
	const wchar_t* CLASS_NAME = L"IMAGE WINDOW CLASS";

	UnregisterClass(CLASS_NAME, m_hInstance);
}

bool Window::ProcessMessages()
{
	MSG msg = {

	};
	while (PeekMessage(&msg, nullptr, 0u, 0u, PM_REMOVE)) 
	{
		if (msg.message == WM_QUIT)
		{
			return false;
		}
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (!IsWindow(m_hWnd))
	{
		return false;
	}
	return true;
}

void Window::CreateMenuBar()
{
	hMenu = CreateMenu();
	hFileMenu = CreateMenu();
	hEditMenu = CreateMenu();
	hOpenSubMenu = CreateMenu();

	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");

	AppendMenu(hFileMenu, MF_STRING, 1, L"New");

	AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hOpenSubMenu, L"Open");
	AppendMenu(hOpenSubMenu, MF_STRING, NULL, L"SubMenu Item");

	AppendMenu(hFileMenu, MF_STRING, 3, L"Save");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, 4, L"Exit");


	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, L"Edit");

	AppendMenu(hEditMenu, MF_STRING, 5, L"Cut");
	AppendMenu(hEditMenu, MF_STRING, 6, L"Copy");
	AppendMenu(hEditMenu, MF_STRING, 7, L"Paste");

	AppendMenu(hMenu, MF_POPUP, 8, L"Help");

	SetMenu(m_hWnd, hMenu);
}

void Window::AddControls()
{
	HWND hStatic = CreateWindowEx(0, L"static", L"Enter text here:", WS_VISIBLE | WS_CHILD, 200, 100, 100, 50, m_hWnd, NULL, m_hInstance, NULL);
}



