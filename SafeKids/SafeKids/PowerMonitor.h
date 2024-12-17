#pragma once
#include <windows.h>
#include <iostream>
#include <chrono>

class PowerMonitor {
public:
	PowerMonitor();
	~PowerMonitor();
	DWORD GetIdleTime();
	bool IsSystemIdle();
	void TimeRunningCalc();
	void SetTimeUseLimit(DWORD dwTimeUseLimit);
	SYSTEMTIME GetCurrentLocalTime();
	static void ThreadCheckUseLimitTime(LPARAM lParam);
	void StartThreadPowerMonitor();
private:
	DWORD dwTimeUseLimit;
};



