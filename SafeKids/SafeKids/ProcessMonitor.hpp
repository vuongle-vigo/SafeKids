#ifndef PROCESS_MONITOR_HPP
#define PROCESS_MONITOR_HPP

#include <Windows.h>
#include <psapi.h> 
#include <tlhelp32.h>
#include <string>
#include <vector>
#include "Common.h"

class ProcessMonitor {
public:
	std::string GetActiveWindowProcessPath();
	std::wstring GetActiveWindowTitle();
	void MonitorProcessUsage();
	//void ListRunningProcesses();
	BOOL StopProcess(std::string &sProcessName);
	std::string GetProcessPath(DWORD dwProcessId);
private:
	typedef struct _ProcessInfo {
		std::string m_sProcessPath;
		std::wstring m_wsProcessTitle;
		double m_fTimeUsage;
	} ProcessInfo, *PProcessInfo;

	ProcessInfo m_processInfo;
	double m_fTimeDelayQuery = 6000; // 60 seconds
};

#endif