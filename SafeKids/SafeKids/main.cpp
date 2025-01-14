#include "Client2Server.h"
#include "PowerMonitor.h"
#include "AppMonitor.hpp"
#include "ProcessMonitor.hpp"
#include "ComputerInfo.h"
#include "Config.h"
#include "HttpClient.h"
#include "json.hpp"

using json = nlohmann::json;

void threadAutoConnectServer() {
	Client2Server& c2s = Client2Server::GetInstance();

	while (1) {
		if (!c2s.GetFlagConnected()) {
			std::cout << "Trying reconnect after 30 seconds..." << std::endl;
			c2s.Connect();
		}
		else {
			std::cout << "Check connect: is connected" << std::endl;
		}
		DEBUG_LOG("check while");
		Sleep(10000);
	}
}

int main() {

	std::thread t(threadAutoConnectServer);

	t.join();

	/*AppMonitor appMonitor;
	appMonitor.ListInstalledApplications();
	appMonitor.DisplayApplications();*/
	//
	/*Client2Server c2s;
	c2s.Connect();
	Sleep(6);
	
	getchar();*/

	/*ComputerInfo com;
	
	std::cout << com.GetMachineGUID();
	json jsRegister;
	jsRegister["type"] = "REGISTER";
	jsRegister["clientId"] = com.GetMachineGUID();
	
	c2s.SendMessage2Server(jsRegister.dump());*/
	std::cout << "Exiting....";
	/*while (1) {
		Sleep(1000);
	}*/
	//ProcessMonitor processMonitor;
	////processMonitor.ListRunningProcesses();
	//std::string currentProcess;
	//while (1) {
	//	if (currentProcess != processMonitor.GetActiveWindowProcessName()) {
	//		std::cout << processMonitor.GetActiveWindowProcessName();
	//		currentProcess = processMonitor.GetActiveWindowProcessName();
	//		c2s.SendMessage2Server(currentProcess);
	//	}

	//	Sleep(1000);
	//}
	//

	/*ComputerInfo com;
	std::cout << com.m_szSerial;*/

	//Config& conf = Config::GetInstance();
	//std::cout << conf.GetHost();

	

	/*PowerMonitor pwm;
	pwm.StartThreadPowerMonitor();*/



	//while (1) {

	//}

	//HttpClient httpClient;
	//httpClient.SendRequestGetToken("admin", "password123");
}

