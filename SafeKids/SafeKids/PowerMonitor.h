#pragma once
#include <windows.h>
#include <iostream>
#include <chrono>

class PowerMonitor {
public:
	PowerMonitor();
	~PowerMonitor();
	static PowerMonitor& GetInstance();
	void SetTimeUseLimit(DWORD dwTimeUseLimit);
	void MonitorPowerUsage();
private:
	DWORD dwTimeUseLimit;
};



