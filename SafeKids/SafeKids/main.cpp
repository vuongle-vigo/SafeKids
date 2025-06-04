//#include "Client2Server.h"
#include "HttpClient.h"
#include "PowerMonitor.h"
#include "AppMonitor.h"
#include "ProcessMonitor.hpp"
#include "ComputerInfo.h"
#include "Config.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include "SQLiteDB.h"
#include "Login.h"
#include "Communication.h"

using json = nlohmann::json;

void ThreadMonitorProcessUsage();
void ThreadGetConfig();
void ThreadMonitorPowerUsage();
void ThreadMonitorApp();

int main() {
	LoginDB& sqlite = LoginDB::GetInstance();
	while (sqlite.getToken() == "") {
		Login& login = Login::GetInstance();
		login.Install();
	}

	HttpClient& httpClient = HttpClient::GetInstance();

	//InitSelfProtectDriver();

	////thread 
	std::thread configThread(ThreadGetConfig);
	Sleep(1000);
	std::thread appMonitorThread(ThreadMonitorApp);
	std::thread powerUsageThread(ThreadMonitorPowerUsage);
	std::thread processUsageThread(ThreadMonitorProcessUsage);

	while (1) {
		PowerUsageDB& powerUsageDB = PowerUsageDB::GetInstance();
		httpClient.SendRequestUpdateOnline();
		json jsonData = powerUsageDB.query_all();
		if (httpClient.PushPowerUsage(jsonData)) {
			powerUsageDB.update_status(jsonData); // Update data after pushing to server
		}

		ProcessUsageDB& processUsageDB = ProcessUsageDB::GetInstance();
		json processData = processUsageDB.query_all();
		if (httpClient.PushProcessUsage(processData)) {
			processUsageDB.update_status(processData); // Update data after pushing to server
		}

		std::this_thread::sleep_for(std::chrono::minutes(1)); 
		std::cout << "Updating power and process usage data..." << std::endl;
	}
}

void ThreadMonitorProcessUsage() {
	ProcessMonitor processMonitor;
	processMonitor.MonitorProcessUsage();
}

void ThreadMonitorPowerUsage() {
	PowerMonitor powerMonitor;
	powerMonitor.MonitorPowerUsage();
}

void ThreadGetConfig() {
	HttpClient& httpClient = HttpClient::GetInstance();
	json config = httpClient.SendRequestGetConfig();
	ConfigMonitorDB& configMonitorDB = ConfigMonitorDB::GetInstance();
	std::string time_limit_daily = config["time_limit_daily"].dump();
	std::string config_websites = config["config_websites"].dump();
	std::string config_apps = config["config_apps"].dump();
	std::string status = config["status"].dump();
	configMonitorDB.add(time_limit_daily, config_websites, config_apps, status);
	json configJson = configMonitorDB.query_config();
	std::cout << "Config: " << configJson.dump(4) << std::endl;
	ConfigMonitor& configMonitor = ConfigMonitor::GetInstance();
	if (!configMonitor.SetConfig(configJson[0])) {
		std::cerr << "Failed to set config" << std::endl;
	}

	std::cout << "Config updated successfully." << std::endl;
	std::this_thread::sleep_for(std::chrono::minutes(1)); // Sleep for 1 minute before next iteration
}

void ThreadMonitorApp() {
	AppMonitor appMonitor;
	appMonitor.MonitorApp();
}