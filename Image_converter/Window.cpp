#include <Windows.h>
#include <commdlg.h>
#include <map>
#include <string>
#include <opencv2/opencv.hpp>


LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void open_file(HWND hWnd);
void ShowHelpContent(HWND hWnd);
BOOL CopyFileToClipboard(HWND hWnd);
BOOL PasteFileFromClipboard(HWND hWnd);
BOOL CutFileFromClipboard(HWND hWnd);
HWND hPictureControl;
HWND hLogo;
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
    void loadImages();
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
        case 8:
            ShowHelpContent(hWnd);
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
        break;

    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;

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
    ofn.lpstrFilter = L"bmp\0*.bmp;*.jpg;*.png\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;

    GetOpenFileName(&ofn);

    if (ofn.nFileOffset == 0) {
        MessageBox(hWnd, L"Failed to select File", L"Error", MB_OK | MB_ICONERROR);
    }
    else {
        HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

        if (hBitmap == NULL) {
            DWORD error = GetLastError();
            LPWSTR errorText = nullptr;
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&errorText,
                0,
                NULL
            );
            MessageBox(hWnd, errorText, L"Error", MB_OK | MB_ICONERROR);
            LocalFree(errorText);
        }
        else {
            // Convert HBITMAP to cv::Mat
            BITMAP bm;
            GetObject(hBitmap, sizeof(BITMAP), &bm);
            HDC hdc = GetDC(NULL);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
            cv::Mat image(bm.bmHeight, bm.bmWidth, CV_8UC3);
            for (int y = 0; y < bm.bmHeight; ++y) {
                for (int x = 0; x < bm.bmWidth; ++x) {
                    COLORREF color = GetPixel(hdcMem, x, y);
                    image.at<cv::Vec3b>(y, x) = cv::Vec3b(GetBValue(color), GetGValue(color), GetRValue(color));
                }
            }
            SelectObject(hdcMem, hOldBitmap);
            DeleteDC(hdcMem);
            ReleaseDC(NULL, hdc);

            // Display the image using OpenCV
            cv::namedWindow("Image", cv::WINDOW_NORMAL);
            cv::imshow("Image", image);
            cv::waitKey(0);
        }
    }
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
    loadImages();
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
    HWND hStatic = CreateWindowEx(0, L"static", L"Open your Picture here:", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 200, 100, 100, 50, m_hWnd, NULL, m_hInstance, NULL);
    HWND hEdit = CreateWindowEx(0, L"Edit", L"Edit text here ...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE, 200, 252, 100, 50, m_hWnd, NULL, m_hInstance, NULL);
    HWND hButton = CreateWindowEx(0, L"button", L"Browse", WS_VISIBLE | WS_CHILD, 200, 152, 100, 50, m_hWnd, (HMENU)10, m_hInstance, NULL);
    hPictureControl = CreateWindowEx(0, L"static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP | SS_CENTERIMAGE, 200, 202, 400, 300, m_hWnd, NULL, m_hInstance, NULL);
    hLogo = CreateWindowEx(0, L"Static", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 350, 60, 100, 100, m_hWnd, NULL, m_hInstance, NULL);
}
void Window::loadImages()
{
    std::wstring imagePath = L"E:\Prog stuff\Visualstudio\Image_converter\Image_converter\Logo.bmp";
    HBITMAP hLogoImage = (HBITMAP)LoadImageW(NULL, imagePath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    SendMessage(hLogo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hLogoImage);
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
void ShowHelpContent(HWND hWnd)
{
    const wchar_t* helpContent = L" Image Converter \n Made by Oskars \n cc 2023";

    // Create a new window to display the help content
    HWND hHelpWnd = CreateWindowEx(
        0,
        L"STATIC",
        L"Help",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL,
        NULL,
        GetModuleHandle(nullptr),
        NULL
    );

    // Create a static control to show the help text
    HWND hHelpText = CreateWindowEx(
        0,
        L"STATIC",
        helpContent,
        WS_VISIBLE | WS_CHILD | SS_CENTER,
        10, 10, 380, 280,
        hHelpWnd,
        NULL,
        GetModuleHandle(nullptr),
        NULL
    );

    // Show the help window
    ShowWindow(hHelpWnd, SW_SHOW);
}
