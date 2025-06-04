#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "SQLiteDB.h"

class Config {
public:
	static Config& GetInstance();
	bool SetWorkDir(const std::string& szNewWorkDir);
	std::wstring GetWorkdir();
	std::string GetHost();
	int GetPort();
private:
	std::wstring m_wszWorkdir;
	std::string m_szServerHost;
	int m_serverPort;

	Config();
	~Config();
	Config(const Config&) = delete;  
	Config& operator=(const Config&) = delete;
};

class ConfigMonitor {
public:
    // Define nested structs first to ensure they are available
    struct TimeRange {
        std::string start; // e.g., "08:00"
        std::string end;   // e.g., "12:00"
    };

    struct DailyLimit {
        std::vector<TimeRange> allowed_time; // List of allowed time ranges
        float max_hours;                    // Max usage hours per day
    };

    struct AppInfo {
        std::string app_id; // e.g., "discord.exe"
        std::string name;   // e.g., "Discord"
    };

    struct AppConfig {
        std::vector<AppInfo> blocked;   // List of blocked apps
        std::vector<AppInfo> uninstall; // List of apps to uninstall
    };

    struct ConfigData {
        std::vector<DailyLimit> time_limit_daily; // Daily limits for 7 days (Monday to Sunday)
        AppConfig config_apps;            // Blocked and uninstall apps
        std::string updated_at;           // Timestamp, e.g., "2025-06-04 01:19:00"
        std::string status;               // Status, e.g., "success"
    };

    static ConfigMonitor& GetInstance();
    bool SetConfig(json config);
    ConfigData GetConfig();
    json GetTodayConfig();
private:
    ConfigData m_configData;

    ConfigMonitor();
    ~ConfigMonitor();
};