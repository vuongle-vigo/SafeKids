#include "Client2Server.h"
#include "AppMonitor.hpp"
#include "ProcessMonitor.hpp"
#include "ComputerInfo.h"
#include "Config.h"

int main() {
	/*AppMonitor appMonitor;
	appMonitor.ListInstalledApplications();
	appMonitor.DisplayApplications();*/
	
	/*ProcessMonitor processMonitor;
	processMonitor.ListRunningProcesses();*/
	//processMonitor.GetActiveWindowProcessName();

	/*ComputerInfo com;
	std::cout << com.m_szSerial;*/

	//Config& conf = Config::GetInstance();
	//std::cout << conf.GetHost();

	Client2Server c2s;
	c2s.Connect();
	std::cout << "hi";
	Sleep(5);
	std::string msg = "Hello vuongle";
	c2s.SendMessage2Server(msg);

	while (1) {

	}
}

