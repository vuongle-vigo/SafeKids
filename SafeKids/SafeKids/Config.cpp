#include "Config.h"
#include "Common.h"
#include <chrono>
#include <ctime>

#define _CRT_SECURE_NO_WARNINGS

Config::Config() {
	m_wszWorkdir = GetCurrentDir();
	m_szServerHost = "192.168.0.101";
	m_serverPort = 8889;
}

Config::~Config() {

}

Config& Config::GetInstance() {
	static Config instance;
	return instance;
}

std::wstring Config::GetWorkdir() {
	return m_wszWorkdir;
}

std::string Config::GetHost() {
	return m_szServerHost;
}

int Config::GetPort() {
	return m_serverPort;
}

ConfigMonitor::ConfigMonitor() {

}

ConfigMonitor::~ConfigMonitor() {

}

ConfigMonitor& ConfigMonitor::GetInstance() {
	static ConfigMonitor instance;
	return instance;
}

ConfigMonitor::ConfigData ConfigMonitor::GetConfig() {
	return m_configData;
}

bool ConfigMonitor::SetConfig(json config) {
    try {
        // Validate top-level JSON object
        if (!config.is_object()) {
            return false;
        }

        ConfigData newConfig;

        // Required fields
        if (!config.contains("time_limit_daily") || !config.contains("config_apps") ||
            !config.contains("updated_at") || !config.contains("status")) {
            return false;
        }

        // Parse updated_at and status
        newConfig.updated_at = config["updated_at"].get<std::string>();
        newConfig.status = nlohmann::json::parse(config["status"].get<std::string>()).get<std::string>(); // Parse escaped JSON string

        // Parse time_limit_daily (JSON string)
        auto time_limit_json = nlohmann::json::parse(config["time_limit_daily"].get<std::string>());
        if (!time_limit_json.is_object()) {
            return false;
        }

        // Order days: Monday to Sunday
        std::vector<std::string> days = { "monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday" };
        for (const auto& day : days) {
            if (!time_limit_json.contains(day)) {
                continue; // Skip missing days
            }
            const auto& day_json = time_limit_json[day];
            if (!day_json.contains("allowed_time") || !day_json.contains("max_hours")) {
                return false;
            }

            DailyLimit daily_limit;
            daily_limit.max_hours = day_json["max_hours"].get<float>();

            // Parse allowed_time (array of strings like "09:00-12:00")
            if (!day_json["allowed_time"].is_array()) {
                return false;
            }
            for (const auto& time_range_str : day_json["allowed_time"]) {
                std::string time_str = time_range_str.get<std::string>();
                // Split "HH:MM-HH:MM" into start and end
                size_t dash_pos = time_str.find('-');
                if (dash_pos == std::string::npos) {
                    return false;
                }
                TimeRange time_range;
                time_range.start = time_str.substr(0, dash_pos); // e.g., "09:00"
                time_range.end = time_str.substr(dash_pos + 1);  // e.g., "12:00"
                daily_limit.allowed_time.push_back(time_range);
            }

            newConfig.time_limit_daily.push_back(daily_limit);
        }

        // Parse config_apps (JSON string)
        auto config_apps_json = nlohmann::json::parse(config["config_apps"].get<std::string>());
        if (!config_apps_json.is_object() || !config_apps_json.contains("blocked") || !config_apps_json.contains("uninstall")) {
            return false;
        }

        // Parse blocked apps
        if (!config_apps_json["blocked"].is_array()) {
            return false;
        }
        for (const auto& app_json : config_apps_json["blocked"]) {
            if (!app_json.contains("app_id") || !app_json.contains("app_name")) {
                return false;
            }
            AppInfo app;
            app.app_id = app_json["app_id"].get<std::string>();
            app.name = app_json["app_name"].get<std::string>();
            newConfig.config_apps.blocked.push_back(app);
        }

        // Parse uninstall apps
        if (!config_apps_json["uninstall"].is_array()) {
            return false;
        }
        for (const auto& app_json : config_apps_json["uninstall"]) {
            if (!app_json.contains("quiet_uninstall_string") || !app_json.contains("app_name")) {
                return false;
            }
            AppInfo app;
            app.app_id = app_json["quiet_uninstall_string"].get<std::string>();
            app.name = app_json["app_name"].get<std::string>();
            newConfig.config_apps.uninstall.push_back(app);
        }

        // Handle config_websites (string "null" or potential list)
        // Since ConfigData doesn't have a config_websites field, ignore it
        // If needed, add to ConfigData and parse here

		// Parse config command if it exists
		if (config.contains("command") && config["command"].is_string()) {
			newConfig.command = config["command"].get<std::string>();
		}
		else {
			newConfig.command = ""; // Default to empty if not present
		}

        // Update m_configData
        m_configData = std::move(newConfig);
        return true;
    }
    catch (const std::exception& e) {
        // Handle JSON parsing or type errors
        return false;
    }
}

json ConfigMonitor::GetTodayConfig() {
    json result;

    // Get current day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    struct tm time_info;
    localtime_s(&time_info, &current_time); 

    int day_of_week = time_info.tm_wday; // 0 = Sunday, 1 = Monday, ..., 6 = Saturday

    // Adjust to match time_limit_daily order (Monday = 0, ..., Sunday = 6)
    int config_index = (day_of_week + 6) % 7; // Converts Sunday (0) to 6, Monday (1) to 0, etc.
	std::cout << "Config index for today: " << config_index << std::endl;
    // Check if config exists for today
    if (config_index >= 0 && static_cast<size_t>(config_index) < m_configData.time_limit_daily.size()) {
        const DailyLimit& today_limit = m_configData.time_limit_daily[config_index];
		std::cout << "Today's limit: " << today_limit.max_hours << " hours" << std::endl;
        // Build JSON result
        result["max_hours"] = today_limit.max_hours;
        json allowed_time_array = json::array();
        for (const auto& time_range : today_limit.allowed_time) {
            json range = {
                {"start", time_range.start},
                {"end", time_range.end}
            };
            allowed_time_array.push_back(range);
        }
        result["allowed_time"] = allowed_time_array;
    }

    return result;
}
