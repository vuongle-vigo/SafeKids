#ifndef APP_MONITOR_HPP
#define APP_MONITOR_HPP

#include <windows.h>
#include <string>
#include <vector>


class AppMonitor {
public:
    void ListInstalledApplications();
    BOOL UninstallApplication(const std::wstring& wsAppName);
    void DisplayApplications();


private:
    struct AppInfo {
        std::string m_sAppName;
    };

    typedef AppInfo* AppInfoPtr;

    std::vector<AppInfo> m_vAppInfo;
};



#endif