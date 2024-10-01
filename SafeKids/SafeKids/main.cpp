#include "AppMonitor.hpp"



int main() {
	AppMonitor appMonitor;
	appMonitor.ListInstalledApplications();
	appMonitor.UninstallApplication(L"Google Chrome Dev");
	//appMonitor.DisplayApplications();
}