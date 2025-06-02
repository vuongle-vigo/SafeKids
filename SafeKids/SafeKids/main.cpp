//#include "Client2Server.h"
#include "HttpClient.h"
#include "PowerMonitor.h"
#include "AppMonitor.hpp"
#include "ProcessMonitor.hpp"
#include "ComputerInfo.h"
#include "Config.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include "SQLiteDB.h"
#include "Login.h"
#include "Communication.h"

using json = nlohmann::json;

int main() {
	//PowerMonitor pm;
	//pm.TimeRunningCalc();
	//ProcessMonitor pm;
	//pm.MonitorProcessUsage();

	//PowerUsageDB& powerUsageDB = PowerUsageDB::GetInstance();
	//json jsonData = powerUsageDB.QueryAllTime();
	//std::cout << jsonData.dump(5);
	//HttpClient& httpClient = HttpClient::GetInstance();
	//httpClient.PushPowerUsage(jsonData);
	//Config& cfg = Config::GetInstance();
	//std::wcout << L"Workdir: " << cfg.GetWorkdir() << std::endl;
	LoginDB& sqlite = LoginDB::GetInstance();
	while (sqlite.getToken() == "") {
		Login& login = Login::GetInstance();
		login.Install();
	}

	//InitSelfProtectDriver();

}

