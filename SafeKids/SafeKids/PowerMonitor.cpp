#include "PowerMonitor.h"
#include "Config.h"
#include "Common.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include "SQLiteDB.h"
#include <thread>
#include "SafeKidsTray.h"

#define _CRT_SECURE_NO_WARNINGS

PowerMonitor::PowerMonitor() {
	dwTimeUseLimit = 123;
}

PowerMonitor::~PowerMonitor() {

}

PowerMonitor& PowerMonitor::GetInstance() {
	static PowerMonitor instance;
	return instance;
}

void PowerMonitor::SetTimeUseLimit(DWORD dwTimeUseLimit) {
	dwTimeUseLimit = dwTimeUseLimit;
}

void PowerMonitor::MonitorPowerUsage() {
	ConfigMonitor& configMonitor = ConfigMonitor::GetInstance();
	json config = configMonitor.GetTodayConfig();
    PowerUsageDB& powerUsageDB = PowerUsageDB::GetInstance();
	SafeKidsTray& safeKidsTray = SafeKidsTray::GetInstance();

    while (true) {
		std::cout << "Monitoring power usage..." << std::endl;
        int current_hour = atoi(GetCurrentTimeHour().c_str());
        int current_minute = atoi(GetCurrentTimeMinute().c_str());
        std::ostringstream current_time_ss;
        current_time_ss << std::setw(2) << std::setfill('0') << current_hour << ":"
            << std::setw(2) << std::setfill('0') << current_minute;
        std::string current_time_str = current_time_ss.str(); // e.g., "03:05"
        // Get today's config
        json config = configMonitor.GetTodayConfig();
        if (config.is_null() || !config.contains("allowed_time") || !config.contains("max_hours")) {
            // No config: assume no restrictions
            std::this_thread::sleep_for(std::chrono::minutes(1));
            continue;
        }

        float max_hours = config["max_hours"].get<float>();
        bool within_allowed_time = false;

		std::cout << "Current Time: " << current_time_str << std::endl;

        // Check if current time is within allowed ranges
        for (const auto& range : config["allowed_time"]) {
            if (!range.contains("start") || !range.contains("end")) {
                continue;
            }
            std::string start_time = range["start"].get<std::string>(); // e.g., "09:00"
            std::string end_time = range["end"].get<std::string>();     // e.g., "13:00"
			std::cout << "Checking range: " << start_time << " - " << end_time << std::endl;
            // Compare times (HH:MM format)
            if (current_time_str >= start_time && current_time_str <= end_time) {
                within_allowed_time = true;
                break;
            }
        }

        // Calculate total usage time for today
        double total_usage_minutes = powerUsageDB.query_today();
		double total_usage_hours = total_usage_minutes / 60.0;

		std::cout << "Total Usage Today: " << total_usage_hours << " hours" << std::endl;

        // Check for violations and show warnings
        if (!within_allowed_time) {
            /*std::thread([]() {
                MessageBoxW(
                    NULL,
                    L"Warning: System usage is outside allowed time ranges!",
                    L"Usage Restriction",
                    MB_OK | MB_ICONWARNING
                );
                }).detach();*/
			safeKidsTray.SendMessageToTray(L"Warning: System usage is outside allowed time ranges!");
        }

        if (total_usage_hours > max_hours) {
            /*std::thread([]() {
                MessageBoxW(
                    NULL,
                    L"Warning: Daily usage limit exceeded!",
                    L"Usage Restriction",
                    MB_OK | MB_ICONWARNING
                );
                }).detach();*/
			safeKidsTray.SendMessageToTray(L"Warning: Daily usage limit exceeded!");
        }

        // Sleep for 1 minute before next check
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}

