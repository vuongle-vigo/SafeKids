#include "Client2Server.h"
#include "PowerMonitor.h"
#include "AppMonitor.hpp"
#include "ProcessMonitor.hpp"
#include "ComputerInfo.h"
#include "Config.h"

int main() {
	/*AppMonitor appMonitor;
	appMonitor.ListInstalledApplications();
	appMonitor.DisplayApplications();*/
	
	Client2Server c2s;
	c2s.Connect();
	Sleep(6);
	
	ComputerInfo com;
	std::cout << com.m_szSerial;
	c2s.SendMessage2Server(com.m_szSerial);

	ProcessMonitor processMonitor;
	//processMonitor.ListRunningProcesses();
	std::string currentProcess;
	while (1) {
		if (currentProcess != processMonitor.GetActiveWindowProcessName()) {
			std::cout << processMonitor.GetActiveWindowProcessName();
			currentProcess = processMonitor.GetActiveWindowProcessName();
			c2s.SendMessage2Server(currentProcess);
		}

		Sleep(1000);
	}
	

	/*ComputerInfo com;
	std::cout << com.m_szSerial;*/

	//Config& conf = Config::GetInstance();
	//std::cout << conf.GetHost();

	

	/*PowerMonitor pwm;
	pwm.StartThreadPowerMonitor();*/



	//while (1) {

	//}
}

