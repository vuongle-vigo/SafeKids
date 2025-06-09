#include "Common.h"
#include <iostream>
#include <cstdio>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <algorithm>
#include <ctime>
#include <fstream>

std::string GetCurrentDate() {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;

    // Use localtime_s instead of localtime
    if (localtime_s(&now_tm, &now_time) != 0) {
        std::cerr << "Failed to get local time" << std::endl;
        return "";
    }

    // Format date and time
    std::ostringstream dateStream, timeStream;
    dateStream << std::put_time(&now_tm, "%Y-%m-%d");
	return dateStream.str();
}

std::string GetCurrentTimeHour() {
	// Get current time
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm now_tm;
	// Use localtime_s instead of localtime
	if (localtime_s(&now_tm, &now_time) != 0) {
		std::cerr << "Failed to get local time" << std::endl;
		return "";
	}
	// Format date and time
	std::ostringstream timeStream;
	timeStream << std::put_time(&now_tm, "%H:%M");
	return timeStream.str();
}

std::string GetCurrentTimeMinute() {
	// Get current time
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm now_tm;
	// Use localtime_s instead of localtime
	if (localtime_s(&now_tm, &now_time) != 0) {
		std::cerr << "Failed to get local time" << std::endl;
		return "";
	}
	// Format date and time
	std::ostringstream timeStream;
	timeStream << std::put_time(&now_tm, "%M");
	return timeStream.str();
}

int ConvertStringToInt(const std::string& str) {
	try {
		return std::stoi(str);
	}
	catch (const std::invalid_argument&) {
		std::cerr << "Invalid argument: " << str << " is not a valid integer." << std::endl;
		return 0;
	}
	catch (const std::out_of_range&) {
		std::cerr << "Out of range: " << str << " is too large to fit in an int." << std::endl;
		return 0;
	}
}

std::wstring DosPathToNtPath(const std::wstring& dosPath) {
	if (dosPath.length() < 2 || dosPath[1] != L':') {
		std::wcerr << L"Invalid DOS path: " << dosPath << std::endl;
		return L"";
	}

	wchar_t drive[3] = { dosPath[0], dosPath[1], L'\0' };
	wchar_t devicePath[512];
	if (QueryDosDeviceW(drive, devicePath, 512) == 0) {
		std::wcerr << L"QueryDosDeviceW failed for drive " << drive << L", error: " << GetLastError() << std::endl;
		return L"";
	}

	std::wstring rest = dosPath.substr(2);
	return std::wstring(devicePath) + rest;
}

std::wstring GetCurrentDir() {
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	for (int i = wcslen(path) - 1; i >= 0; --i) {
		if (path[i] == '\\') {
			path[i + 1] = '\0'; 
			break;
		}
	}

	return std::wstring(path);
}

std::wstring GetCurrentProcessPath() {
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	return std::wstring(path);
}

std::string WstringToString(const std::wstring& wstr) {
	if (wstr.empty()) return "";
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string result(size - 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &result[0], size, nullptr, nullptr);
	return result;
}

std::wstring StringToWstring(const std::string& str) {
	if (str.empty()) return L"";
	int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::wstring result(size - 1, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
	return result;
}

std::wstring RemoveQuotesW(const std::wstring& input) {
	std::wstring result = input;
	if (!result.empty() && result.front() == L'"' && result.back() == L'"') {
		result = result.substr(1, result.size() - 2);
	}
	return result;
}

std::string RemoveQuotes(const std::string& input) {
	std::string result = input;
	if (!result.empty() && result.front() == '"' && result.back() == '"') {
		result = result.substr(1, result.size() - 2);
	}
	return result;
}

std::wstring ToLowercaseW(const std::wstring& input) {
	std::wstring result = input;
	std::transform(result.begin(), result.end(), result.begin(), ::towlower);
	return result;
}

std::string ToLowercase(const std::string& input) {
	std::string result = input;
	std::transform(result.begin(), result.end(), result.begin(), ::tolower);
	return result;
}

std::string RemoveTrailingSplash(const std::string& input) {
	if (!input.empty() && input.back() == '\\') {
		return input.substr(0, input.length() - 1);
	}
	return input;
}

void LogToFile(const std::string& message, const std::wstring& filePath) {
	std::wstring logDir = GetCurrentDir() + L"\\" + LOG_FILE;
	std::ofstream logFile(filePath, std::ios_base::app);
	if (logFile.is_open()) {
		logFile << GetCurrentDate() << " " << GetCurrentTimeHour() << ": " << message << std::endl;
		logFile.close();
	}
}

#include <wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
bool StartProcessInUserSession(const std::wstring& applicationPath) {
	DWORD sessionId = WTSGetActiveConsoleSessionId();
	if (sessionId == 0xFFFFFFFF) return false;

	HANDLE hToken;
	if (!WTSQueryUserToken(sessionId, &hToken)) {
		return false;
	}

	STARTUPINFOW si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.lpDesktop = (LPWSTR)L"WinSta0\\Default";

	BOOL success = CreateProcessAsUserW(
		hToken, applicationPath.c_str(), NULL, NULL, NULL, FALSE,
		NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi
	);

	CloseHandle(hToken);
	if (success) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	return success;
}

bool DeleteOwnService(const wchar_t* serviceName) {
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager) {
		std::wcerr << L"Failed to open SCM: " << GetLastError() << std::endl;
		return false;
	}

	SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_ALL_ACCESS);
	if (!hService) {
		std::wcerr << L"Failed to open service: " << GetLastError() << std::endl;
		CloseServiceHandle(hSCManager);
		return false;
	}

	// D?ng service n?u ?ang ch?y
	SERVICE_STATUS status;
	if (ControlService(hService, SERVICE_CONTROL_STOP, &status)) {
		std::wcout << L"Service stopped successfully." << std::endl;
	}

	// Xóa service
	if (DeleteService(hService)) {
		std::wcout << L"Service deleted successfully." << std::endl;
	}
	else {
		std::wcerr << L"Failed to delete service: " << GetLastError() << std::endl;
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return false;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	return true;
}

void UninstallSelfProtectDriver(const std::wstring& serviceName) {
	std::wofstream batFile(L"uninstall_filter_driver.bat");
	batFile << L"@echo off\n";
	//batFile << L"fltmc unload \"" << serviceName << L"\"\n";
	batFile << L"sc stop \"" << serviceName << L"\"\n"; // Stop service
	batFile << L"sc delete \"" << serviceName << L"\"\n";   
	//batFile << L":repeat\n";
	//batFile << L"del \"" << sysFilePath << L"\"\n";        
	//batFile << L"if exist \"" << sysFilePath << L"\" goto repeat\n";
	batFile << L"del \"%~f0\"\n";                           
	batFile.close();

	ShellExecuteW(NULL, L"open", L"uninstall_filter_driver.bat", NULL, NULL, SW_HIDE);
}