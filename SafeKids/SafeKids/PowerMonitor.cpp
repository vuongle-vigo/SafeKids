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