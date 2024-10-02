#ifndef PROCESS_MONITOR_HPP
#define PROCESS_MONITOR_HPP

#include <Windows.h>
#include <psapi.h> 
#include <tlhelp32.h>
#include <string>
#include <vector>
#include "common.cpp"

class ProcessMonitor {
public:
	std::string GetActiveWindowProcessName();
	void ListRunningProcesses();
	BOOL StopProcess(std::string &sProcessName);

private:
	std::string GetProcessPath(DWORD dwProcessId);

	struct ProcessInfo {
		std::string m_sProcessName;
		std::string m_sProcessPath;
	};
	typedef struct ProcessInfo* PProcessInfo;

	std::vector<ProcessInfo> m_vProcessInfo;
};

#endif