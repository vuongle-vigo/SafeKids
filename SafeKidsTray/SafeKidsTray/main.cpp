#include "PipeConnection.h"
#include "ProcessMonitor.h"
#include <thread>
#include <string>
#include <iostream>

#define PRORCESS_LABLE L"PROCESS"

void ThreadProcessMonitor() {
	PipeConnection& pipeConnection = PipeConnection::GetInstance();
	while (1) {
		std::string processPath = GetActiveWindowProcessPath();
		std::wstring windowTitle = GetActiveWindowTitle();
		std::cout << "Active Window Process Path: " << processPath << std::endl;
		std::wcout << L"Active Window Title: " << windowTitle << std::endl;
		if (!processPath.empty() && !windowTitle.empty()) {
			std::wstring message = std::wstring(PRORCESS_LABLE) + L"|" + windowTitle + L"|" + std::wstring(processPath.begin(), processPath.end()) + L"\0";
			std::wcout << L"Sending message to server: " << message << std::endl;
			if (!pipeConnection.SendMessageToServer(message)) {
				std::cerr << "Failed to send message to server." << std::endl;
			}
			else {
				std::cout << "Message sent successfully." << std::endl;
			}
		}
		std::this_thread::sleep_for(std::chrono::seconds(3)); // Sleep for 1 second
	}
}

int main() {
	// Initialize the pipe connection
	PipeConnection& pipeConnection = PipeConnection::GetInstance();
	std::thread monitorThread(ThreadProcessMonitor);
	if (pipeConnection.InitPipe()) {
		// Pipe initialized successfully
		// You can add additional logic here if needed
		
	}
	else {
		// Handle pipe initialization failure
		// For example, you could log an error or show a message box
	}
	return 0;
}