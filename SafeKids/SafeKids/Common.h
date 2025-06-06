#pragma once
#include <string>
#include <iostream>

#define LOG_FILE L"SafeKids.log"

#ifdef _DEBUG
#define PRINT_API_ERR(API_NAME) \
	std::cout << API_NAME << ": " << GetLastError() << std::endl;
#else
#define PRINT_API_ERR(API_NAME) 
#endif

#ifdef _DEBUG
#define DEBUG_LOG(fmt, ...) \
    std::printf(fmt, __VA_ARGS__); \
    std::printf("\n");
#else
#define DEBUG_LOG(fmt, ...)
#endif

std::string GetCurrentDate();
std::string GetCurrentTimeHour();
std::string GetCurrentTimeMinute();
int ConvertStringToInt(const std::string& str);
std::wstring DosPathToNtPath(const std::wstring& dosPath);
std::wstring GetCurrentDir();
std::wstring GetCurrentProcessPath();
std::string WstringToString(const std::wstring& wstr);
std::wstring StringToWstring(const std::string& str);
std::wstring RemoveQuotesW(const std::wstring& input);
std::string RemoveQuotes(const std::string& input);
std::wstring ToLowercaseW(const std::wstring& input);
std::string ToLowercase(const std::string& input);
std::string RemoveTrailingSplash(const std::string& input);
void LogToFile(const std::string& message, const std::wstring& filePath = LOG_FILE);
bool StartProcessInUserSession(const std::wstring& applicationPath);