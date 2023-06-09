#include <Windows.h>
#include <commdlg.h>
#include <map>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void open_file(HWND hWnd);
BOOL CopyFileToClipboard(HWND hWnd);
BOOL PasteFileFromClipboard(HWND hWnd);
BOOL CutFileFromClipboard(HWND hWnd);

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
        case 5:
            CutFileFromClipboard(hWnd);
            break;
        case 6:
            CopyFileToClipboard(hWnd);
            break;
        case 7:
            PasteFileFromClipboard(hWnd);
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
    HWND hEdit = CreateWindowEx(0, L"Edit", L"Edit text here ...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 200, 252, 100, 50, m_hWnd, NULL, m_hInstance, NULL);
    HWND hButton = CreateWindowEx(0, L"button", L"Browse", WS_VISIBLE | WS_CHILD, 200, 152, 100, 50, m_hWnd, (HMENU)10, m_hInstance, NULL);
}

BOOL CopyFileToClipboard(HWND hWnd)
{
    if (!OpenClipboard(hWnd))
    {
        MessageBox(hWnd, L"Failed to open clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (!EmptyClipboard())
    {
        MessageBox(hWnd, L"Failed to empty clipboard.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        MessageBox(hWnd, L"No marked text available for copying.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    if (!OpenClipboard(hWnd))
    {
        MessageBox(hWnd, L"Failed to open clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (hClipboardData == NULL)
    {
        MessageBox(hWnd, L"Failed to get clipboard data.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    wchar_t* lpClipboardText = (wchar_t*)GlobalLock(hClipboardData);
    if (lpClipboardText == NULL)
    {
        MessageBox(hWnd, L"Failed to lock clipboard data.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    size_t clipboardTextLength = wcslen(lpClipboardText);
    HGLOBAL hCopyData = GlobalAlloc(GMEM_MOVEABLE, (clipboardTextLength + 1) * sizeof(wchar_t));
    if (hCopyData == NULL)
    {
        MessageBox(hWnd, L"Failed to allocate memory for copy data.", L"Error", MB_OK | MB_ICONERROR);
        GlobalUnlock(hClipboardData);
        CloseClipboard();
        return FALSE;
    }

    wchar_t* lpCopyData = (wchar_t*)GlobalLock(hCopyData);
    if (lpCopyData == NULL)
    {
        MessageBox(hWnd, L"Failed to lock copy data.", L"Error", MB_OK | MB_ICONERROR);
        GlobalUnlock(hClipboardData);
        CloseClipboard();
        return FALSE;
    }

    wcscpy_s(lpCopyData, clipboardTextLength + 1, lpClipboardText);

    GlobalUnlock(hCopyData);
    GlobalUnlock(hClipboardData);

    if (!SetClipboardData(CF_UNICODETEXT, hCopyData))
    {
        MessageBox(hWnd, L"Failed to set clipboard data.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    CloseClipboard();

    MessageBox(hWnd, L"Text copied to clipboard.", L"Success", MB_OK | MB_ICONINFORMATION);

    return TRUE;
}

BOOL PasteFileFromClipboard(HWND hWnd)
{
    if (!OpenClipboard(hWnd))
    {
        MessageBox(hWnd, L"Failed to open clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        MessageBox(hWnd, L"No text available for pasting.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (hClipboardData == NULL)
    {
        MessageBox(hWnd, L"Failed to get clipboard data.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    wchar_t* lpClipboardText = (wchar_t*)GlobalLock(hClipboardData);
    if (lpClipboardText == NULL)
    {
        MessageBox(hWnd, L"Failed to lock clipboard data.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    // Do something with the pasted text

    GlobalUnlock(hClipboardData);
    CloseClipboard();

    MessageBox(hWnd, L"Text pasted from clipboard.", L"Success", MB_OK | MB_ICONINFORMATION);

    return TRUE;
}

BOOL CutFileToClipboard(HWND hWnd)
{
    return 0;
}

BOOL CutFileFromClipboard(HWND hWnd)
{
    if (!OpenClipboard(hWnd))
    {
        MessageBox(hWnd, L"Failed to open clipboard.", L"Error", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        MessageBox(hWnd, L"No text available for cutting.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (hClipboardData == NULL)
    {
        MessageBox(hWnd, L"Failed to get clipboard data.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    wchar_t* lpClipboardText = (wchar_t*)GlobalLock(hClipboardData);
    if (lpClipboardText == NULL)
    {
        MessageBox(hWnd, L"Failed to lock clipboard data.", L"Error", MB_OK | MB_ICONERROR);
        CloseClipboard();
        return FALSE;
    }

    // Do something with the cut text

    GlobalUnlock(hClipboardData);
    CloseClipboard();

    MessageBox(hWnd, L"Text cut from clipboard.", L"Success", MB_OK | MB_ICONINFORMATION);

    return TRUE;
}
