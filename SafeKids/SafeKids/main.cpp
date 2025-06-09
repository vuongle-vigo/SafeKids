#include "Client2Server.h"
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
#include <windows.h>
#include <string>
#include <thread>
#include "SafeKidsTray.h"

using json = nlohmann::json;

// Define thread functions
void ThreadMonitorProcessUsage();
void ThreadMonitorPowerUsage();
void ThreadGetConfig();
void ThreadMonitorApp();
void ThreadSafeKidsTray();

// Global for service variable
SERVICE_STATUS g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_ServiceStatusHandle = NULL;
bool g_RunningAsService = false;

// Login main
void RunMainLogic() {
    LogToFile("SafeKids started...");
    LoginDB& sqlite = LoginDB::GetInstance();

	// Wait for login token
    while (sqlite.getToken() == "") {
        LogToFile("Waiting for login token...");
        Login& login = Login::GetInstance();
        login.Install();
    }

    LogToFile("Login successful.");

    //InitSelfProtectDriver();    

    HttpClient& httpClient = HttpClient::GetInstance();
	SafeKidsTray& safeKidsTray = SafeKidsTray::GetInstance();
	//safeKidsTray.InitPipeServer();
    std::thread safeKidsTrayThread(ThreadSafeKidsTray);
	// Create and start threads for various tasks
    std::thread configThread(ThreadGetConfig);
    Sleep(1000);
    std::thread appMonitorThread(ThreadMonitorApp);
    std::thread powerUsageThread(ThreadMonitorPowerUsage);
    std::thread processUsageThread(ThreadMonitorProcessUsage);
	safeKidsTray.SendMessageToTray(L"SafeKids service started successfully.");
	//DeleteOwnService(L"SafeKidsService");
	//UninstallSelfProtectDriver(L"SelfProtectWDM");
	// Main loop for service
    while (g_ServiceStatus.dwCurrentState == SERVICE_RUNNING) {
        PowerUsageDB& powerUsageDB = PowerUsageDB::GetInstance();
        httpClient.SendRequestUpdateOnline();
        json jsonData = powerUsageDB.query_all();
        if (httpClient.PushPowerUsage(jsonData)) {
            powerUsageDB.update_status(jsonData);
        }

        ProcessUsageDB& processUsageDB = ProcessUsageDB::GetInstance();
        json processData = processUsageDB.query_all();
        if (httpClient.PushProcessUsage(processData)) {
            processUsageDB.update_status(processData);
        }

        std::this_thread::sleep_for(std::chrono::minutes(1));
        LogToFile("Updating power and process usage data...");
    }

	// Wait for threads to finish
    configThread.join();
    appMonitorThread.join();
    powerUsageThread.join();
    processUsageThread.join();
}

// Function controller for the service
void WINAPI ServiceCtrlHandler(DWORD dwControl) {
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
        LogToFile("Service stopped.");
        break;
    default:
        SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
        break;
    }
}

// Service main function
void WINAPI ServiceMain(DWORD argc, LPSTR* argv) {
    g_ServiceStatusHandle = RegisterServiceCtrlHandlerA("SafeKidsService", ServiceCtrlHandler);
    if (!g_ServiceStatusHandle) {
        LogToFile("Failed to register service control handler: " + std::to_string(GetLastError()));
        return;
    }

	// Setup service status
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;
    g_ServiceStatus.dwWaitHint = 0;

    SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

    // Set to running status
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

    LogToFile("Service started.");

    // Run main logic
    RunMainLogic();
}

// Class ServiceManager to handle service creation
class ServiceManager {
public:
    static bool CreateService() {
        SC_HANDLE schSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
        if (schSCManager == NULL) {
            LogToFile("Failed to open Service Control Manager: " + std::to_string(GetLastError()));
            return false;
        }

		// Check if the service already exists
        SC_HANDLE schServiceCheck = OpenServiceA(schSCManager, "SafeKidsService", SERVICE_QUERY_STATUS);
        if (schServiceCheck != NULL) {
            LogToFile("Service already exists.");
            CloseServiceHandle(schServiceCheck);
            CloseServiceHandle(schSCManager);
			return true; // Service already exists, no need to create again
        }

		// Get the path of the current executable
        char szPath[MAX_PATH];
        if (GetModuleFileNameA(NULL, szPath, MAX_PATH) == 0) {
            LogToFile("Failed to get module file name: " + std::to_string(GetLastError()));
            CloseServiceHandle(schSCManager);
            return false;
        }

		// Add the --service argument to the path
        std::string servicePath = std::string(szPath) + " --service";
        if (servicePath.length() >= MAX_PATH) {
            LogToFile("Service path too long after adding --service.");
            CloseServiceHandle(schSCManager);
            return false;
        }

		// Create the service
        SC_HANDLE schService = CreateServiceA(
            schSCManager,
            "SafeKidsService",
            "SafeKids Monitoring Service",
            SERVICE_ALL_ACCESS,
            SERVICE_WIN32_OWN_PROCESS,
            SERVICE_AUTO_START,
            SERVICE_ERROR_NORMAL,
            servicePath.c_str(),
            NULL, NULL, NULL, NULL, NULL
        );

        if (schService == NULL) {
            DWORD error = GetLastError();
            LogToFile("Failed to create service: " + std::to_string(error));
            CloseServiceHandle(schSCManager);
            return false;
        }

		// Set service description
        SERVICE_DESCRIPTIONA description = { (LPSTR)"SafeKids monitoring service for tracking system usage." };
        ChangeServiceConfig2A(schService, SERVICE_CONFIG_DESCRIPTION, &description);

        LogToFile("Service created successfully with --service argument.");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return true;
    }
};

int main(int argc, char* argv[]) {
	// Check if running as a service
    if (argc > 1 && std::string(argv[1]) == "--service") {
        g_RunningAsService = true;
        SERVICE_TABLE_ENTRYA serviceTable[] = {
            { (LPSTR)"SafeKidsService", ServiceMain },
            { NULL, NULL }
        };
        if (!StartServiceCtrlDispatcherA(serviceTable)) {
            LogToFile("Failed to start service dispatcher: " + std::to_string(GetLastError()));
            return 1;
        }
    }
    else {
        // Run in console mode to debug
        LogToFile("Running in console mode...");

        LoginDB& sqlite = LoginDB::GetInstance();
        while (sqlite.getToken() == "") {
            LogToFile("Waiting for login token...");
            Login& login = Login::GetInstance();
            login.Install();
        }

		// Create service if not already created
        LogToFile("Login successful. Attempting to create service...");
        if (!ServiceManager::CreateService()) {
            LogToFile("Service creation failed.");
        }
        else {
            LogToFile("Service setup completed successfully.");
        }

		// Run logic in console mode
        g_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
        RunMainLogic();
    }

    return 0;
}

void ThreadMonitorProcessUsage() {
    ProcessMonitor &processMonitor = ProcessMonitor::GetInstance();
    processMonitor.MonitorProcessUsage();
}

void ThreadMonitorPowerUsage() {
    PowerMonitor powerMonitor;
    powerMonitor.MonitorPowerUsage();
}

void ThreadGetConfig() {
    HttpClient& httpClient = HttpClient::GetInstance();
    while (1) {
        json config = httpClient.SendRequestGetConfig();
        ConfigMonitorDB& configMonitorDB = ConfigMonitorDB::GetInstance();
        std::string time_limit_daily = config["time_limit_daily"].dump();
        std::string config_websites = config["config_websites"].dump();
        std::string config_apps = config["config_apps"].dump();
		std::string command = config["command"].is_string() ? config["command"].get<std::string>() : "";
        std::string status = config["status"].dump();
        configMonitorDB.add(time_limit_daily, config_websites, config_apps, command, status);
        json configJson = configMonitorDB.query_config();
        std::cout << "Config: " << configJson.dump(4) << std::endl;
        ConfigMonitor& configMonitor = ConfigMonitor::GetInstance();
        if (!configMonitor.SetConfig(configJson[0])) {
            std::cerr << "Failed to set config" << std::endl;
        }
        else {
            std::cout << "Config updated successfully." << std::endl;
        }

        if (RemoveQuotes(command) == "uninstall") {
			LogToFile("Uninstall command received. Uninstalling SafeKids...");
			DeleteOwnService(L"SafeKidsService");
			UninstallSelfProtectDriver(L"SelfProtectWDM");
            httpClient.SendRequestUninstall();
			SafeKidsTray& safeKidsTray = SafeKidsTray::GetInstance();
			safeKidsTray.SendMessageToTray(L"Uninstalling SafeKids service...");
			exit(0); // Exit the application after uninstalling
        }

        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

void ThreadMonitorApp() {
    AppMonitor appMonitor;
    appMonitor.MonitorApp();
}

void ThreadSafeKidsTray() {
	SafeKidsTray& safeKidsTray = SafeKidsTray::GetInstance();
	safeKidsTray.InitPipeServer();
}