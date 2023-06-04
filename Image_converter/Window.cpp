#include <Windows.h>
#include <commdlg.h>
#include <map>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void open_file(HWND hWnd);

class Window
{
public:
    Window();
    ~Window();
    bool ProcessMessages();

private:
    HINSTANCE m_hInstance;
    HWND m_hWnd;
    HMENU hMenu;
    HMENU hFileMenu;
    HMENU hEditMenu;
    HMENU hOpenSubMenu;

    void CreateMenuBar();
    void AddControls();
};

std::map<HWND, Window*> windowMap;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;

    while (window.ProcessMessages())
    {
        // Main message loop
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1:
            MessageBeep(MB_ICONINFORMATION);
            break;
        case 2:
            break;
        case 3:
            DestroyWindow(hWnd);
            break;
        case 10:
            open_file(hWnd);
            break;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY:
        windowMap.erase(hWnd);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void open_file(HWND hWnd)
{
    OPENFILENAME ofn;
    wchar_t file_name[100];

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = file_name;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = 100;
    ofn.lpstrFilter = L"All files\0*.*\0Source Files\0*.CPP\0Text Files\0*.TXT\0";
    ofn.nFilterIndex = 1;

    GetOpenFileName(&ofn);

    // Display the selected file path (optional)
    MessageBox(hWnd, ofn.lpstrFile, L"Selected File", MB_OK);
}

Window::Window()
    : m_hInstance(GetModuleHandle(nullptr))
{
    const wchar_t* CLASS_NAME = L"IMAGE_WINDOW_CLASS";

    WNDCLASS wndClass = {};
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hInstance = m_hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.lpszClassName = CLASS_NAME;

    RegisterClass(&wndClass);

    DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

    int width = 640;
    int height = 480;

    RECT rect;
    rect.left = 250;
    rect.top = 250;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;

    AdjustWindowRect(&rect, style, false);

    m_hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"Title",
        style,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL,
        NULL,
        m_hInstance,
        NULL
    );

    windowMap[m_hWnd] = this;
    CreateMenuBar();
    AddControls();
    ShowWindow(m_hWnd, SW_SHOW);
}

Window::~Window()
{
    const wchar_t* CLASS_NAME = L"IMAGE_WINDOW_CLASS";
    UnregisterClass(CLASS_NAME, m_hInstance);
}

bool Window::ProcessMessages()
{
    MSG msg;
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
    HWND hStatic = CreateWindowEx(0, L"static", L"Paste your Picture here:", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 200, 100, 100, 50, m_hWnd, NULL, m_hInstance, NULL);
    HWND hButton = CreateWindowEx(0, L"button", L"Browse", WS_VISIBLE | WS_CHILD, 200, 152, 100, 50, m_hWnd, (HMENU)10, m_hInstance, NULL);
}
