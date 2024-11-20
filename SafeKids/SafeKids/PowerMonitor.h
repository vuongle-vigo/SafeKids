#pragma once

#include <windows.h>
#include <iostream>

class PowerMonitor {
public:
	PowerMonitor();
	~PowerMonitor();
	void SetTimeUseLimit(DWORD dwTimeUseLimit);
	SYSTEMTIME GetCurrentLocalTime();
	static void ThreadCheckUseLimitTime(LPARAM lParam);
	void StartThreadPowerMonitor();
private:
	DWORD dwTimeUseLimit;
};



