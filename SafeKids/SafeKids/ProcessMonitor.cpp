#include "ProcessMonitor.hpp"
#include "SQLiteDB.h"
#include "Config.h"

std::string ProcessMonitor::GetActiveWindowProcessPath() {
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

std::wstring ProcessMonitor::GetActiveWindowTitle() {
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

//void ProcessMonitor::ListRunningProcesses() {
//    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//    if (hProcessSnap == INVALID_HANDLE_VALUE) {
//        PRINT_API_ERR("CreateToolhelp32Snapshot");
//        return;
//    }
//
//    PROCESSENTRY32 pe32;
//    pe32.dwSize = sizeof(PROCESSENTRY32);
//
//    if (Process32First(hProcessSnap, &pe32)) {
//        do {
//            std::string processPath = this->GetProcessPath(pe32.th32ProcessID);
//            ProcessMonitor::ProcessInfo processInfo;
//#ifdef UNICODE
//            std::wstring ws(pe32.szExeFile);
//            std::string processName(ws.begin(), ws.end());
//#else
//            std::string processName(pe32.szExeFile);      
//#endif
//            processInfo.m_sProcessName = processName;
//            processInfo.m_sProcessPath = processPath;
//
//            this->m_vProcessInfo.push_back(processInfo);
//        } while (Process32Next(hProcessSnap, &pe32));
//    }
//    else {
//        PRINT_API_ERR("Process32First");
//    }
//
//    CloseHandle(hProcessSnap);
//}

std::string ProcessMonitor::GetProcessPath(DWORD dwProcessId) {
    TCHAR processName[MAX_PATH] = TEXT("<unknown>");

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
    if (hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
            GetModuleFileNameEx(hProcess, hMod, processName, sizeof(processName) / sizeof(TCHAR));
        }

        CloseHandle(hProcess);
    }

#ifdef UNICODE
    std::wstring ws(processName);
    return std::string(ws.begin(), ws.end());
#else
    return std::string(processName);
#endif
}

BOOL ProcessMonitor::StopProcess(std::string& sProcessName) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        PRINT_API_ERR("CreateToolhelp32Snapshot");
        return false;
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
            if (processName == sProcessName) {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                if (hProcess == NULL) {
                    PRINT_API_ERR("OpenProcess");
                    CloseHandle(hProcessSnap);
                    return false;
                }

                if (!TerminateProcess(hProcess, 0)) {
                    PRINT_API_ERR("TerminateProcess");
                    CloseHandle(hProcess);
                    CloseHandle(hProcessSnap);
                    return false;
                }

                CloseHandle(hProcess);
                CloseHandle(hProcessSnap);
                return true;
            }

        } while (Process32Next(hProcessSnap, &pe32));
    }
    else {
        PRINT_API_ERR("Process32First");
    }

    CloseHandle(hProcessSnap);
    return false;
}

bool ProcessMonitor::CheckBlockApp(std::string &sProcessPath) {
	ConfigMonitor& configMonitor = ConfigMonitor::GetInstance();
	ConfigMonitor::ConfigData configData = configMonitor.GetConfig();
	ConfigMonitor::AppConfig configApps = configData.config_apps;
    std::string path = ToLowercase(RemoveQuotes(sProcessPath));
	for (const auto& app : configApps.blocked) {
		std::string pathCheck = ToLowercase(RemoveQuotes(app.app_id));
		if (path.find(pathCheck) == 0) {
			std::wcout << L"Blocked App Detected: " << pathCheck.c_str() << std::endl;
			MessageBoxA(
				NULL,
				(std::string("Blocked App Detected: ") + pathCheck).c_str(),
				"App Blocked",
				MB_OK | MB_ICONWARNING
			);
			return true; // Blocked app found
		}
	}
}

void ProcessMonitor::MonitorProcessUsage() {
	ProcessUsageDB& processUsageDB = ProcessUsageDB::GetInstance();
	PowerUsageDB& powerUsageDB = PowerUsageDB::GetInstance();
    while (1) {
		std::wstring wsActiveWindowTitle = GetActiveWindowTitle();
        CheckBlockApp(m_processInfo.m_sProcessPath);
		if (m_processInfo.m_wsProcessTitle != wsActiveWindowTitle) {
			m_processInfo.m_wsProcessTitle = wsActiveWindowTitle;
			//std::wcout << L"Active Window Title: " << m_processInfo.m_wsProcessTitle << std::endl;
            m_processInfo.m_sProcessPath = GetActiveWindowProcessPath();
			m_processInfo.m_fTimeUsage = 0;
            //Insert new database
			processUsageDB.add(
                m_processInfo.m_wsProcessTitle,
				m_processInfo.m_sProcessPath,
				GetCurrentDate(),
				GetCurrentTimeHour(),
				m_processInfo.m_fTimeUsage
			);
        }
        else {
			m_processInfo.m_fTimeUsage += m_fTimeDelayQuery / 60000; // Convert milliseconds to minutes
			std::wcout << L"Process Path: " << m_processInfo.m_sProcessPath.c_str() << L", Time Usage: " << m_processInfo.m_fTimeUsage << L" minutes" << std::endl;
            //Update database
            processUsageDB.update_lastest(
                m_processInfo.m_wsProcessTitle,
                m_processInfo.m_sProcessPath,
                GetCurrentDate(),
                GetCurrentTimeHour(),
                m_processInfo.m_fTimeUsage
            );
        }
        
        // Check AppLock.exe app isn't active
		if (m_processInfo.m_sProcessPath.find("LockApp.exe") == std::string::npos) {
			std::string currentDate = GetCurrentDate();
			int currentHour = ConvertStringToInt(GetCurrentTimeHour());
			double usage_minutes = powerUsageDB.QueryByTime(currentDate, currentHour);
            if (usage_minutes != -1) {
                double new_usage_minutes = usage_minutes + m_fTimeDelayQuery / 60000; // Convert milliseconds to minutes
                if (!powerUsageDB.update(currentDate, currentHour, new_usage_minutes)) {
					std::cerr << "Failed to update power usage in database." << std::endl;
                }
            }
            else {
                if (!powerUsageDB.add(currentDate, currentHour, m_fTimeDelayQuery / 60000)) {
					std::cerr << "Failed to add power usage in database." << std::endl;
                } 
            }
		}

        Sleep((int)m_fTimeDelayQuery);
    }
}