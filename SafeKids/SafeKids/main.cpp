//#include "Client2Server.h"
#include "PowerMonitor.h"
#include "AppMonitor.hpp"
#include "ProcessMonitor.hpp"
//#include "ComputerInfo.h"
//#include "Config.h"
#include "HttpClient.h"
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
	//std::string sqlitePath = "C:\\Users\\levuong\\Documents\\GitHub\\SafeKids\\SafeKids\\sqlite_db\\safekids.db";	
	//SQLiteDB db(sqlitePath);
	//db.execute("CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, name TEXT);");

	//PowerMonitor pm;
	//pm.TimeRunningCalc();
	ProcessMonitor pm;
	pm.MonitorProcessUsage();
	
}

