#include "AppMonitor.hpp"
#include "Common.h"

void AppMonitor::ListInstalledApplications() {
    HKEY hKeyEnumApp[2] = { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER };
    const wchar_t* registryPaths[2] = { L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 
        L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall" };
    for (HKEY hKeyRoot : hKeyEnumApp) {
        for (const wchar_t* registryPath : registryPaths) {
            HKEY hKey;
            if (RegOpenKeyEx(hKeyRoot, registryPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
                PRINT_API_ERR("RegOpenKeyEx");
                continue;
            }

            DWORD index = 0;
            TCHAR subKeyName[256];
            DWORD subKeyNameSize = 256;

            while (RegEnumKeyEx(hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                HKEY hSubKey;
                if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                    TCHAR displayName[256];
                    DWORD bufferSize = sizeof(displayName);

                    if (RegQueryValueEx(hSubKey, TEXT("DisplayName"), NULL, NULL, (LPBYTE)displayName, &bufferSize) == ERROR_SUCCESS) {
                        AppMonitor::AppInfo appInfo;
#ifdef UNICODE
                        std::wstring wDisplayName(displayName);
                        std::string appName(wDisplayName.begin(), wDisplayName.end());
#else
                        std::string appName(displayName);
#endif
                        appInfo.m_sAppName = appName;
                        this->m_vAppInfo.push_back(appInfo);
                    }

                    RegCloseKey(hSubKey);
                }

                index++;
                subKeyNameSize = 256;
            }

            RegCloseKey(hKey);
        }
    }
}

BOOL AppMonitor::UninstallApplication(const std::wstring& wsAppName) {
    HKEY hKeyEnumApp[2] = { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER };
    const wchar_t* registryPaths[2] = { L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall",
        L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall" };
    for (HKEY hKeyRoot : hKeyEnumApp) {
        for (const wchar_t* registryPath : registryPaths) {
            HKEY hKey;
            if (RegOpenKeyEx(hKeyRoot, registryPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
                PRINT_API_ERR("RegOpenKeyEx");
                continue;
            }

            DWORD index = 0;
            TCHAR subKeyName[256];
            DWORD subKeyNameSize = 256;
            while (RegEnumKeyEx(hKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
                HKEY hSubKey;
                if (RegOpenKeyEx(hKey, subKeyName, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                    TCHAR displayName[256];
                    TCHAR uninstallString[512];
                    DWORD bufferSize = sizeof(displayName);

                    if (RegQueryValueEx(hSubKey, L"DisplayName", NULL, NULL, (LPBYTE)displayName, &bufferSize) == ERROR_SUCCESS) {
                        std::wstring wDisplayName(displayName);
                        if (wDisplayName == wsAppName) {
                            bufferSize = sizeof(uninstallString);
                            if (RegQueryValueEx(hSubKey, L"UninstallString", NULL, NULL, (LPBYTE)uninstallString, &bufferSize) == ERROR_SUCCESS) {
                                std::wcout << L"Uninstalling: " << wDisplayName << std::endl;
                                std::wcout << L"UninstallString: " << uninstallString << std::endl;

                                STARTUPINFO si = { sizeof(si) };
                                si.wShowWindow = SW_HIDE; 
                                PROCESS_INFORMATION pi;
                                if (CreateProcess(NULL, uninstallString, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
                                    WaitForSingleObject(pi.hProcess, INFINITE);
                                    CloseHandle(pi.hProcess);
                                    CloseHandle(pi.hThread);

                                    std::wcout << L"Uninstall successed" << std::endl;
                                    RegCloseKey(hSubKey);
                                    RegCloseKey(hKey);
                                    return true;
                                }
                                else {
                                    std::wcerr << L"Error uninstall with error code: " << GetLastError() << std::endl;
                                }
                            }
                        }
                    }

                    RegCloseKey(hSubKey);
                }

                index++;
                subKeyNameSize = 256;
            }
        }
    }
}

void AppMonitor::DisplayApplications() {
    for (AppMonitor::AppInfo appInfo : this->m_vAppInfo) {
        std::cout << appInfo.m_sAppName << std::endl;
    }
}