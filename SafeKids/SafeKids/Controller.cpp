//#include "Client2Server.h"
//#include "PowerMonitor.h"
//#include "AppMonitor.hpp"
//#include "ProcessMonitor.hpp"
//#include "ComputerInfo.h"
//#include "Config.h"
//#include "HttpClient.h"
//#include "json.hpp"
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
//		
//		Sleep(10000);
//	}
//}
//
//void createThreadConnectServer() {
//	
//
//
//}