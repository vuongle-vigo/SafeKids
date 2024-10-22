#include "AppMonitor.hpp"
#include "ProcessMonitor.hpp"


int main() {
	/*AppMonitor appMonitor;
	appMonitor.ListInstalledApplications();
	appMonitor.DisplayApplications();*/
	
	ProcessMonitor processMonitor;
	processMonitor.ListRunningProcesses();
	//processMonitor.GetActiveWindowProcessName();
}