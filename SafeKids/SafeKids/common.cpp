#include "Common.h"
#include <iostream>
#include <cstdio>
#include <chrono>
#include <sstream>
#include <iomanip>

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