#pragma once
#include <string>
#include <iostream>

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
int ConvertStringToInt(const std::string& str);