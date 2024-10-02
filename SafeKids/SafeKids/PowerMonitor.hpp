#ifndef POWER_MONITOR_HPP
#define POWER_MONITOR_HPP

#include <windows.h>
#include <wtsapi32.h>
#include <chrono>

class PowerMonitor {
public:
	PowerMonitor();


private:
	HWND hwnd;
	std::chrono::system_clock::time_point loginTime;


};




#endif // !POWER_MONITOR_HPP
