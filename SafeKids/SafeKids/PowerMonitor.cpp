#include "PowerMonitor.h"


PowerMonitor::PowerMonitor() {
	dwTimeUseLimit = 123;
}

PowerMonitor::~PowerMonitor() {

}

PowerMonitor& PowerMonitor::GetInstance() {
	static PowerMonitor instance;
	return instance;
}

void PowerMonitor::SetTimeUseLimit(DWORD dwTimeUseLimit) {
	dwTimeUseLimit = dwTimeUseLimit;
}

