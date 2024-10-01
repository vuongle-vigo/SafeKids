#include "ProcessMonitor.hpp"

std::string ProcessMonitor::GetActiveWindowProcessName() {
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

void ProcessMonitor::ListRunningProcesses() {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        PRINT_API_ERR("CreateToolhelp32Snapshot");
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            std::string processPath = this->GetProcessPath(pe32.th32ProcessID);
            ProcessMonitor::ProcessInfo processInfo;
#ifdef UNICODE
            std::wstring ws(pe32.szExeFile);
            std::string processName(ws.begin(), ws.end());
#else
            std::string processName(pe32.szExeFile);      
#endif
            processInfo.m_sProcessName = processName;
            processInfo.m_sProcessPath = processPath;

        } while (Process32Next(hProcessSnap, &pe32));
    }
    else {
        PRINT_API_ERR("Process32First");
    }

    CloseHandle(hProcessSnap);
}

std::string ProcessMonitor::GetProcessPath(DWORD dwProcessId) {
    TCHAR processName[MAX_PATH] = TEXT("<unknown>");

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
    if (hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleFileNameEx(hProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR));
        }
    }

    CloseHandle(hProcess);

#ifdef UNICODE
    std::wstring ws(processName);
    return std::string(ws.begin(), ws.end());
#else
    return std::string(processName);
#endif
}