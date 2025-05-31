//#include "Client2Server.h"
#include "HttpClient.h"
#include "PowerMonitor.h"
#include "AppMonitor.hpp"
#include "ProcessMonitor.hpp"
//#include "ComputerInfo.h"
//#include "Config.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include "SQLiteDB.h"

using json = nlohmann::json;
//
//void threadAutoConnectServer() {
//	Client2Server& c2s = Client2Server::GetInstance();
//
//	while (1) {
//		if (!c2s.GetFlagConnected()) {
//			std::cout << "Trying reconnect after 30 seconds..." << std::endl;
//			c2s.Connect();
//		}
//		else {
//			std::cout << "Check connect: is connected" << std::endl;
//		}
//		DEBUG_LOG("check while");
//		Sleep(10000);
//	}
//}

int main() {
	//PowerMonitor pm;
	//pm.TimeRunningCalc();
	ProcessMonitor pm;
	pm.MonitorProcessUsage();

	//PowerUsageDB& powerUsageDB = PowerUsageDB::GetInstance();
	//json jsonData = powerUsageDB.QueryAllTime();
	//std::cout << jsonData.dump(5);
	//HttpClient& httpClient = HttpClient::GetInstance();
	//httpClient.PushPowerUsage(jsonData);
}

