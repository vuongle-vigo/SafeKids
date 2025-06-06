#include "Login.h"
#include "Common.h"
#include "Config.h"
#include "HttpClient.h"
#include <Windows.h>


Login::Login() {
	Config& cfg = Config::GetInstance();
	m_wszLoginPath = cfg.GetWorkdir() + PROCESS_LOGIN_NAME;
}

Login::~Login() {
	// Destructor implementation
}

Login& Login::GetInstance() {
	static Login instance;
	return instance;
}
#include <sddl.h>
#pragma comment(lib, "advapi32.lib")
bool Login::InitPipeServer() {
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = FALSE;

	PSECURITY_DESCRIPTOR pSD = NULL;
	const char* sddl = "D:(A;;GA;;;WD)";
	if (!ConvertStringSecurityDescriptorToSecurityDescriptorA(sddl, SDDL_REVISION_1, &pSD, NULL)) {
		LogToFile("Failed to create security descriptor: " + std::to_string(GetLastError()));
		return false;
	}

	sa.lpSecurityDescriptor = pSD;

	HANDLE hPipe = CreateNamedPipeW(
		LOGIN_PIPE_NAME,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		1, // Number of instances
		MAX_MESSAGE_SIZE, // Output buffer size
		MAX_MESSAGE_SIZE, // Input buffer size
		0, // Default timeout
		&sa // Default security attributes
	);
	if (hPipe == INVALID_HANDLE_VALUE) {
		PRINT_API_ERR("CreateNamedPipeW");
		return false;
	}

	if (!ConnectNamedPipe(hPipe, NULL)) {
		if (GetLastError() != ERROR_PIPE_CONNECTED) {
			PRINT_API_ERR("ConnectNamedPipe");
			CloseHandle(hPipe);
			return false;
		}
	}

	wchar_t buffer[MAX_MESSAGE_SIZE] = { 0 };
	DWORD bytesRead = 0;
	HttpClient& httpClient = HttpClient::GetInstance();

	while (true) {
		if (!ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
			PRINT_API_ERR("ReadFile");
			break;
		}

		if (bytesRead == 0) {
			std::cout << "Client disconnected." << std::endl;
			break;
		}

		// Here you can process the message and send a response if needed
		std::wstring response = std::wstring(L"failed");
		buffer[bytesRead] = '\0'; // Null-terminate the string
		std::wcout << "Received message: " << buffer << std::endl;
		std::wstring loginInfo(buffer);
		size_t sep = loginInfo.find(L'|');
		if (sep != std::wstring::npos) {
			std::wstring usr = loginInfo.substr(0, sep);
			std::wstring pass = loginInfo.substr(sep + 1);
			std::wcout << L"Part 1: " << usr << std::endl;
			std::wcout << L"Part 2: " << pass << std::endl;
			if (httpClient.SendRequestGetToken(
				std::string(usr.begin(), usr.end()).c_str(),
				std::string(pass.begin(), pass.end()).c_str()
			)) {
				response = L"success";
				DWORD bytesWritten = 0;
				if (!WriteFile(hPipe, response.c_str(), response.size() * 2, &bytesWritten, NULL)) {
					PRINT_API_ERR("WriteFile");
					break;
				}
				break;
			}
			else {
				response = L"failed";
				DWORD bytesWritten = 0;
				if (!WriteFile(hPipe, response.c_str(), response.size() * 2, &bytesWritten, NULL)) {
					PRINT_API_ERR("WriteFile");
					break;
				}
			}
		}
		else {
			std::wcout << L"Cannot find '|'" << std::endl;
		}
	}

	CloseHandle(hPipe);
	return true;
}

bool Login::Install() {
	// Installation logic
	STARTUPINFOW si = { sizeof(si) };
	PROCESS_INFORMATION pi = { 0 };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW; 
	std::cout << "Installing SafeKidsLogin..." << std::endl;
	//if (!CreateProcessW(
	//	m_wszLoginPath.c_str(), // Application name
	//	NULL,                   // Command line arguments
	//	NULL,                   // Process security attributes
	//	NULL,                   // Primary thread security attributes
	//	FALSE,                 // Inherit handles
	//	CREATE_NEW_CONSOLE,                     // Creation flags
	//	NULL,                  // Environment block
	//	NULL,                  // Current directory
	//	&si,
	//	&pi
	//)) {
	//	PRINT_API_ERR("CreateProcessW");
	//	return false;
	//}
	if (!StartProcessInUserSession(m_wszLoginPath)) {
		std::cout << "Failed to start process in user session." << std::endl;
		return false;
	}

	return this->InitPipeServer();
}

bool Login::Uninstall() {
	// Uninstallation logic
	std::cout << "Uninstalling SafeKidsLogin..." << std::endl;
	// Add uninstallation code here
	return true;
}

