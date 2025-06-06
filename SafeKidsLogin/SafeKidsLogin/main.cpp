#include <windows.h>
#include <string>
#include <commctrl.h>
#include <iostream>
#include <wchar.h>

#pragma comment(lib, "comctl32.lib")

// Định nghĩa các control
HWND hUsernameEdit, hPasswordEdit, hLoginButton, hLabel;
HBRUSH hBgBrush;

// Tên pipe
#define PIPE_NAME L"\\\\.\\pipe\\LoginPipe"

// Hàm xử lý placeholder
LRESULT CALLBACK EditProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    static bool isPlaceholder = true;
    if (msg == WM_SETFOCUS && isPlaceholder) {
        SetWindowTextW(hwnd, L"");
        isPlaceholder = false;
    }
    else if (msg == WM_KILLFOCUS) {
        wchar_t buffer[256];
        GetWindowTextW(hwnd, buffer, 256);
        if (wcslen(buffer) == 0) {
            if (hwnd == hUsernameEdit) SetWindowTextW(hwnd, L"Tài khoản");
            else if (hwnd == hPasswordEdit) SetWindowTextW(hwnd, L"Mật khẩu");
            isPlaceholder = true;
        }
    }
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

// Hàm xử lý cửa sổ
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HBRUSH hButtonBrush = CreateSolidBrush(RGB(0, 120, 215)); // Màu xanh cho nút
    static HBRUSH hButtonHoverBrush = CreateSolidBrush(RGB(0, 150, 255)); // Màu hover
    static bool isHovering = false;
    HFONT hFont;
    switch (msg) {
    case WM_CREATE:
        // Tạo brush cho nền
        hBgBrush = CreateSolidBrush(RGB(240, 240, 240)); // Màu nền xám nhạt

        // Thiết lập font Segoe UI
        hFont = CreateFontW(
            16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
        );

        // Tạo nhãn tiêu đề
        hLabel = CreateWindowW(
            L"STATIC", L"Đăng nhập vào hệ thống",
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            50, 10, 200, 20, hwnd, NULL, NULL, NULL
        );
        SendMessage(hLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Tạo trường nhập tài khoản
        hUsernameEdit = CreateWindowW(
            L"EDIT", L"Tài khoản",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP,
            50, 40, 200, 25, hwnd, NULL, NULL, NULL
        );
        SendMessage(hUsernameEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SetWindowSubclass(hUsernameEdit, EditProc, 0, 0);

        // Tạo trường nhập mật khẩu
        hPasswordEdit = CreateWindowW(
            L"EDIT", L"Mật khẩu",
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD | ES_AUTOHSCROLL | WS_TABSTOP,
            50, 80, 200, 25, hwnd, NULL, NULL, NULL
        );
        SendMessage(hPasswordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SetWindowSubclass(hPasswordEdit, EditProc, 0, 0);

        // Tạo nút Đăng nhập
        hLoginButton = CreateWindowW(
            L"BUTTON", L"Đăng nhập",
            WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP,
            100, 120, 100, 30, hwnd, (HMENU)1, NULL, NULL
        );
        SendMessage(hLoginButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        break;

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
        // Màu nền và chữ cho edit và static
        SetBkColor((HDC)wParam, RGB(240, 240, 240));
        SetTextColor((HDC)wParam, RGB(0, 0, 0));
        return (LRESULT)hBgBrush;

    case WM_DRAWITEM: {
        LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
        if (pdis->CtlID == 1) { // Nút Đăng nhập
            // Vẽ nền nút
            HBRUSH hBrush = isHovering ? hButtonHoverBrush : hButtonBrush;
            FillRect(pdis->hDC, &pdis->rcItem, hBrush);

            // Vẽ viền
            FrameRect(pdis->hDC, &pdis->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH));

            // Vẽ chữ
            wchar_t btnText[256];
            GetWindowTextW(hLoginButton, btnText, 256);
            SetBkMode(pdis->hDC, TRANSPARENT);
            SetTextColor(pdis->hDC, RGB(255, 255, 255));
            DrawTextW(pdis->hDC, btnText, -1, &pdis->rcItem, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return TRUE;
        }
        break;
    }

    case WM_MOUSEMOVE: {
        POINT pt = { LOWORD(lParam), HIWORD(lParam) };
        RECT btnRect;
        GetClientRect(hLoginButton, &btnRect);
        MapWindowPoints(hLoginButton, hwnd, (LPPOINT)&btnRect, 2);
        bool wasHovering = isHovering;
        isHovering = PtInRect(&btnRect, pt);
        if (isHovering != wasHovering) {
            InvalidateRect(hLoginButton, NULL, TRUE);
            if (isHovering) {
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hLoginButton, 0 };
                TrackMouseEvent(&tme);
            }
        }
        break;
    }

    case WM_MOUSELEAVE:
        if (isHovering) {
            isHovering = false;
            InvalidateRect(hLoginButton, NULL, TRUE);
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { // Nút Đăng nhập
            // Lấy dữ liệu từ các trường nhập
            wchar_t username[256], password[256];
            GetWindowTextW(hUsernameEdit, username, 256);
            GetWindowTextW(hPasswordEdit, password, 256);

            // Kiểm tra placeholder
            if (wcscmp(username, L"Tài khoản") == 0 || wcscmp(password, L"Mật khẩu") == 0 ||
                wcslen(username) == 0 || wcslen(password) == 0) {
                MessageBoxW(hwnd, L"Vui lòng nhập đầy đủ thông tin!", L"Lỗi", MB_OK | MB_ICONERROR);
                break;
            }

            // Kết nối đến named pipe
            HANDLE hPipe = CreateFileW(
                PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL
            );

            if (hPipe == INVALID_HANDLE_VALUE) {
                MessageBoxW(hwnd, L"Không thể kết nối đến ứng dụng gốc!", L"Lỗi", MB_OK | MB_ICONERROR);
                break;
            }

            // Chuẩn bị dữ liệu: username|password
            std::wstring data = std::wstring(username) + L"|" + std::wstring(password);
            DWORD bytesWritten;

            // Gửi dữ liệu
            BOOL success = WriteFile(
                hPipe, data.c_str(), (data.size() + 1) * sizeof(wchar_t), &bytesWritten, NULL
            );

            if (!success) {
                MessageBoxW(hwnd, L"Gửi dữ liệu thất bại!", L"Lỗi", MB_OK | MB_ICONERROR);
                CloseHandle(hPipe);
                break;
            }

            // Đọc phản hồi từ server
            wchar_t response[256] = { 0 };
			DWORD bytesRead;
            while (1) {
                success = ReadFile(
                    hPipe, response, sizeof(response) - 1, &bytesRead, NULL
                );
                if (!success || bytesRead == 0) {
                    wcscpy_s(response, L"Đăng nhập thất bại!");
                    break;
                }
                else {
                    response[bytesRead / sizeof(wchar_t)] = L'\0'; 
                    if (!wcscmp(response, L"success")) {
                        break;
					}
                    else if (!wcscmp(response, L"failed")) {
                        break;
                    }
                }
            }

			CloseHandle(hPipe);

            MessageBoxW(hwnd, response, L"Thành công", MB_OK | MB_ICONINFORMATION);
            DestroyWindow(hwnd);
        }
        break;

    case WM_DESTROY:
        DeleteObject(hBgBrush);
        DeleteObject(hButtonBrush);
        DeleteObject(hButtonHoverBrush);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Hàm chính
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // Khởi tạo Common Controls
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);

    // Đăng ký lớp cửa sổ
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"LoginWindowClass";
    wc.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Icon mặc định
    RegisterClassW(&wc);

    // Tạo cửa sổ
    HWND hwnd = CreateWindowW(
        L"LoginWindowClass", L"Đăng nhập",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 220,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Vòng lặp thông điệp
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return (int)msg.wParam;
}