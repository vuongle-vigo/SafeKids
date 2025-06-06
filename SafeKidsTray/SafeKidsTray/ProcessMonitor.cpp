#include "ProcessMonitor.h"
#include <psapi.h> 
#include <tlhelp32.h>

std::string GetActiveWindowProcessPath() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) return "";

    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
    if (hProcess == NULL) return "";

    TCHAR processName[MAX_PATH] = TEXT("<unknown>");

    if (GetModuleFileNameEx(hProcess, NULL, processName, MAX_PATH)) {
#ifdef UNICODE
        std::wstring ws(processName);
        std::string processNameStr(ws.begin(), ws.end());
#else
        std::string processNameStr(processName);
#endif
        CloseHandle(hProcess);
        return processNameStr;
    }

    CloseHandle(hProcess);
    return "";
}

std::wstring GetActiveWindowTitle() {
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) return L"";
    TCHAR windowTitle[MAX_PATH];
    GetWindowText(hwnd, windowTitle, MAX_PATH);
#ifdef UNICODE
    std::wstring ws(windowTitle);
    return ws;
#else
    return std::string(windowTitle);
#endif
}