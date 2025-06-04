#ifndef APP_MONITOR_HPP
#define APP_MONITOR_HPP

#include <windows.h>
#include <string>
#include <vector>


class RegistryKey {
public:
    RegistryKey() : m_hKey(nullptr) {}
    explicit RegistryKey(HKEY hKey) : m_hKey(hKey) {}
    ~RegistryKey() { if (m_hKey) RegCloseKey(m_hKey); }
    HKEY* operator&() { return &m_hKey; }
    HKEY get() const { return m_hKey; }
private:
    HKEY m_hKey;
};

class AppMonitor {
public:
    void ListInstalledApplications();
    BOOL UninstallApplication(const std::wstring& wsAppName);
    void DisplayApplications();
    void AddApplicationsToDb();
    bool ExecuteUninstall(const std::string& quietUninstallString);
    bool IsAppInstalled(const std::wstring& appName);
    void MonitorApp();
private:
    struct AppInfo {
        std::wstring m_sAppName;
        std::wstring m_sVersion;
        std::wstring m_sPublisher;
        std::wstring m_sInstallLocation;
        std::wstring m_sExePath; // L?y t? DisplayIcon
        std::wstring m_sUninstallString;
        std::wstring m_sQuietUninstallString;
    };

    typedef AppInfo* AppInfoPtr;

    std::vector<AppInfo> m_vAppInfo;
};



#endif