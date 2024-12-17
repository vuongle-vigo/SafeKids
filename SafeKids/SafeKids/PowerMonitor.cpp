#include "PowerMonitor.h"


PowerMonitor::PowerMonitor() {
	dwTimeUseLimit = 123;
}

PowerMonitor::~PowerMonitor() {

}

void PowerMonitor::SetTimeUseLimit(DWORD dwTimeUseLimit) {
	dwTimeUseLimit = dwTimeUseLimit;
}

SYSTEMTIME PowerMonitor::GetCurrentLocalTime() {
	SYSTEMTIME lt;
	GetLocalTime(&lt);
	return lt;
}

DWORD PowerMonitor::GetIdleTime() {
	LASTINPUTINFO lii;
	lii.cbSize = sizeof(LASTINPUTINFO);
	
	//Get time user don't work
	if (GetLastInputInfo(&lii)) {
		DWORD tickCount = GetTickCount();
		return tickCount - lii.dwTime;
	}
	return 0;
}

bool PowerMonitor::IsSystemIdle() {
	DWORD idleTime = GetIdleTime();
	return idleTime > 10000;
}

void PowerMonitor::TimeRunningCalc() {
	using namespace std::chrono;
	steady_clock::time_point startTime = steady_clock::now();

	while (true) {
		if (!IsSystemIdle()) {
			auto now = steady_clock::now();
			auto elapsed = duration_cast<seconds>(now - startTime).count();
			std::cout << "Windows is running, time: " << elapsed << " seconds\n";
		}
		else {
			std::cout << "Windows is idle.\n";
		}

		Sleep(1000);
	}
}

void PowerMonitor::ThreadCheckUseLimitTime(LPARAM lParam) {
	if (!lParam) {
		return;
	}

	PowerMonitor* pwm = (PowerMonitor*)lParam;
	while (1) {
		SYSTEMTIME lt = pwm->GetCurrentLocalTime();
		DWORD dwTimeMinute = lt.wHour * 60 + lt.wMinute;
		if (dwTimeMinute > pwm->dwTimeUseLimit) {
			MessageBoxA(nullptr, "Limit time use", "Message", MB_OK);
		}

		Sleep(60 * 1000);
	}
}

void PowerMonitor::StartThreadPowerMonitor() {
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ThreadCheckUseLimitTime, this, 0, nullptr);
}