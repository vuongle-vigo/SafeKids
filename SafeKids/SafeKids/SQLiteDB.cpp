#include "SQLiteDB.h"
#include "Common.h"
#include <iostream>

// SQLiteDB class implementation
SQLiteDB::SQLiteDB() {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

SQLiteDB::~SQLiteDB() {
    if (db) {
        sqlite3_close(db);
    }
}

SQLiteDB& SQLiteDB::GetInstance() {
	static SQLiteDB instance;
	return instance;
}

bool SQLiteDB::execute(const std::string& query) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// SQLiteStmt class implementation
SQLiteStmt::SQLiteStmt(SQLiteDB& db, const std::string& query) {
    if (sqlite3_prepare_v2(db.getDB(), query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db.getDB()) << std::endl;
        stmt = nullptr;
    }
}

SQLiteStmt::~SQLiteStmt() {
    if (stmt) {
        sqlite3_finalize(stmt);
    }
}

bool SQLiteStmt::step() {
    return sqlite3_step(stmt) == SQLITE_ROW;
}

// ProcessUsage class implementation
ProcessUsageDB::ProcessUsageDB() : db(SQLiteDB::GetInstance()) {}

ProcessUsageDB::~ProcessUsageDB() {}

ProcessUsageDB& ProcessUsageDB::GetInstance() {
	static ProcessUsageDB instance;
	return instance;
}

bool ProcessUsageDB::add(const std::wstring& process_title, const std::string& process_path, const std::string& date_recorded, const std::string& start_time, double time_usage) {
    const wchar_t* sql = L"INSERT INTO process_usage (process_title, process_path, date_recorded, start_time, time_usage) VALUES (?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare16_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text16(stmt, 1, process_title.c_str(), -1, SQLITE_TRANSIENT); // UTF-16
    sqlite3_bind_text(stmt, 2, process_path.c_str(), -1, SQLITE_TRANSIENT);    // UTF-8
    sqlite3_bind_text(stmt, 3, date_recorded.c_str(), -1, SQLITE_TRANSIENT);   // UTF-8
    sqlite3_bind_text(stmt, 4, start_time.c_str(), -1, SQLITE_TRANSIENT);      // UTF-8
    sqlite3_bind_double(stmt, 5, time_usage);                                     // Double

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    return success;
}

bool ProcessUsageDB::update_lastest(const std::wstring& process_title, const std::string& process_path, const std::string& date_recorded, const std::string& start_time, double time_usage) {
    const wchar_t* sql = L"UPDATE process_usage SET process_title = ?, process_path = ?, date_recorded = ?, start_time = ?, time_usage = ?, upload_status = 0 WHERE usage_id = (SELECT MAX(usage_id) FROM process_usage);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare16_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text16(stmt, 1, process_title.c_str(), -1, SQLITE_TRANSIENT); // UTF-16
    sqlite3_bind_text(stmt, 2, process_path.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
    sqlite3_bind_text(stmt, 3, date_recorded.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
    sqlite3_bind_text(stmt, 4, start_time.c_str(), -1, SQLITE_TRANSIENT);   // UTF-8
    sqlite3_bind_double(stmt, 5, time_usage); // float/double

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    return success;
}


bool ProcessUsageDB::remove(int usage_id) {
    std::string query = "DELETE FROM process_usage WHERE usage_id = " + std::to_string(usage_id) + ";";
    return db.execute(query);
}

json ProcessUsageDB::query_all() {
    json result;
    const char* sql = "SELECT * FROM process_usage WHERE upload_status = 0;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result; // Return empty JSON if query fails
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
		const char* process_title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		const char* process_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		const char* date_recorded = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		const char* start_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		double time_usage = sqlite3_column_double(stmt, 5);
        time_usage = round(time_usage * 10) / 10;
        result.push_back({ {"process_title", process_title}, {"process_path", process_path}, {"date_recorded", date_recorded}, {"start_time", start_time}, {"time_usage", time_usage}});
    }

    sqlite3_finalize(stmt);
    return result;
}

bool ProcessUsageDB::delete_data(json data) {
    if (!data.is_array() || data.empty()) {
        return false; // Return false if input JSON is not an array or is empty
    }

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM process_usage WHERE process_title = ? AND date_recorded = ? AND start_time = ?;";

    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false; // Return false if query preparation fails
    }

    bool success = true;

    // Iterate through JSON array
    for (const auto& item : data) {
        if (!item.contains("process_title") || !item.contains("date_recorded") || !item.contains("start_time")) {
            success = false; // Mark as failed but continue processing other records
            continue;
        }

        std::string process_title = item["process_title"].get<std::string>();
        std::string date_recorded = item["date_recorded"].get<std::string>();
        std::string start_time = item["start_time"].get<std::string>();

        // Bind parameters
        sqlite3_bind_text(stmt, 1, process_title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, date_recorded.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, start_time.c_str(), -1, SQLITE_STATIC);

        // Execute statement
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            success = false; // Mark as failed but continue processing other records
        }

        // Reset statement for next iteration
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    return success;
}

bool ProcessUsageDB::update_status(json data) {
    if (!data.is_array() || data.empty()) {
        return false; // Return false if input JSON is not an array or is empty
    }

    sqlite3_stmt* stmt;
    const char* sql = "UPDATE process_usage SET upload_status = 1 WHERE process_title = ? AND date_recorded = ? AND start_time = ?;";

    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false; // Return false if query preparation fails
    }

    bool success = true;

    // Iterate through JSON array
    for (const auto& item : data) {
        if (!item.contains("process_title") || !item.contains("date_recorded") || !item.contains("start_time")) {
            success = false; // Mark as failed but continue processing other records
            continue;
        }

        std::string process_title = item["process_title"].get<std::string>();
        std::string date_recorded = item["date_recorded"].get<std::string>();
        std::string start_time = item["start_time"].get<std::string>();

        // Bind parameters
        sqlite3_bind_text(stmt, 1, process_title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, date_recorded.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, start_time.c_str(), -1, SQLITE_STATIC);

        // Execute statement
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            success = false; // Mark as failed but continue processing other records
        }

        // Reset statement for next iteration
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    return success;
}

PowerUsageDB::PowerUsageDB() : db(SQLiteDB::GetInstance()) {}

PowerUsageDB& PowerUsageDB::GetInstance() {
	static PowerUsageDB instance;
	return instance;
}

PowerUsageDB::~PowerUsageDB() {}

bool PowerUsageDB::add(const std::string& date, int hour, double usage_minutes) {
	const char* sql = "INSERT INTO power_usage (date, hour, usage_minutes) VALUES (?, ?, ?);";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL Error: " << sqlite3_errmsg(db.getDB()) << std::endl;
		return false;
	}

	sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
	sqlite3_bind_int(stmt, 2, hour); // int
	sqlite3_bind_double(stmt, 3, usage_minutes); // float/double

	bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        std::cerr << "SQL Error: " << sqlite3_errmsg(db.getDB()) << std::endl;
    }
	sqlite3_finalize(stmt);
	return success;
}

bool PowerUsageDB::update(const std::string& date, int hour, double usage_minutes) {
    const char* sql = "UPDATE power_usage SET usage_minutes = ?, upload_status = 0 WHERE date = ? AND hour = ?;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}

    sqlite3_bind_double(stmt, 1, usage_minutes); // float/double
	sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
	sqlite3_bind_int(stmt, 3, hour); // int

	bool success = (sqlite3_step(stmt) == SQLITE_DONE);
	sqlite3_finalize(stmt);
	return success;
}

double PowerUsageDB::QueryByTime(const std::string& date, int hour) {
	const char* sql = "SELECT usage_minutes FROM power_usage WHERE date = ? AND hour = ?;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
		return -1;
	}

	sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
	sqlite3_bind_int(stmt, 2, hour); // int

	double usage_minutes = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
		usage_minutes = sqlite3_column_double(stmt, 0);
		std::cout << "Date: " << date << ", Hour: " << hour << ", Usage Minutes: " << usage_minutes << std::endl;
    }

	sqlite3_finalize(stmt);
	return usage_minutes;
}

json PowerUsageDB::query_all() {
	json result;
	std::string currentDate = GetCurrentDate();
	std::string currentTime = GetCurrentTimeHour();

	const char* sql = "SELECT date, hour, usage_minutes FROM power_usage WHERE upload_status = 0;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
		return result; // Return empty JSON if query fails
	}

	while (sqlite3_step(stmt) == SQLITE_ROW) {
		std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		int hour = sqlite3_column_int(stmt, 1);
		double usage_minutes = sqlite3_column_double(stmt, 2);
        usage_minutes = round(usage_minutes * 10) / 10;
        result.push_back({ {"date", date}, {"hour", hour}, {"usage_minutes", usage_minutes} });
		//if (!(date == currentDate && hour == ConvertStringToInt(currentTime))) {
  //          usage_minutes = round(usage_minutes * 10) / 10;
		//	result.push_back({ {"date", date}, {"hour", hour}, {"usage_minutes", usage_minutes} });
		//}
	}

	sqlite3_finalize(stmt);
	return result;
}

double PowerUsageDB::query_today() {
    std::string currentDate = GetCurrentDate();
    double total_minutes = 0.0;

    const char* sql = "SELECT usage_minutes FROM power_usage WHERE date = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return 0.0; // Return 0.0 if query preparation fails
    }

    // Bind the current date
    sqlite3_bind_text(stmt, 1, currentDate.c_str(), -1, SQLITE_STATIC);

    // Iterate through results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        double usage_minutes = sqlite3_column_double(stmt, 0);
        total_minutes += usage_minutes;
    }

    sqlite3_finalize(stmt);
    return total_minutes;
}

bool PowerUsageDB::delete_data(json data) {
    if (!data.is_array() || data.empty()) {
        return false; // Return false if input JSON is not an array or is empty
    }

    // Get current date and hour
    std::string currentDate = GetCurrentDate();
    int currentHour = ConvertStringToInt(GetCurrentTimeHour());

    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM power_usage WHERE date = ? AND hour = ?;";

    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false; // Return false if query preparation fails
    }

    bool success = true;

    // Iterate through JSON array
    for (const auto& item : data) {
        if (!item.contains("date") || !item.contains("hour")) {
            success = false; // Mark as failed but continue processing other records
            continue;
        }

        std::string date = item["date"].get<std::string>();
        int hour = item["hour"].get<int>();

        // Skip records matching current date and hour
        if (date == currentDate && hour == currentHour) {
            continue;
        }

        // Bind parameters
        sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, hour);

        // Execute statement
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            success = false; // Mark as failed but continue processing other records
        }

        // Reset statement for next iteration
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    return success;
}

bool PowerUsageDB::update_status(json data) {
    if (!data.is_array() || data.empty()) {
        return false; // Return false if input JSON is not an array or is empty
    }

    // Get current date and hour
    std::string currentDate = GetCurrentDate();
    int currentHour = ConvertStringToInt(GetCurrentTimeHour());

    sqlite3_stmt* stmt;
    const char* sql = "UPDATE power_usage SET upload_status = 1 WHERE date = ? AND hour = ?;";

    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false; // Return false if query preparation fails
    }

    bool success = true;

    // Iterate through JSON array
    for (const auto& item : data) {
        if (!item.contains("date") || !item.contains("hour")) {
            success = false; // Mark as failed but continue processing other records
            continue;
        }

        std::string date = item["date"].get<std::string>();
        int hour = item["hour"].get<int>();

        // Skip records matching current date and hour
        if (date == currentDate && hour == currentHour) {
            continue;
        }

        // Bind parameters
        sqlite3_bind_text(stmt, 1, date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, hour);

        // Execute statement
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            success = false; // Mark as failed but continue processing other records
        }

        // Reset statement for next iteration
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    return success;
}

LoginDB::LoginDB() : db(SQLiteDB::GetInstance()) {}

LoginDB::~LoginDB() {}

LoginDB& LoginDB::GetInstance() {
	static LoginDB instance;
	return instance;
}

bool LoginDB::add(const std::string& username, const std::string& password_hash, const std::string& token_encrypted) {
	std::string currentDate = GetCurrentDate();
	std::string currentTime = GetCurrentTimeHour();
	std::string timeLogin = currentDate + " " + currentTime;
	const char* sql = "INSERT INTO user_account (username, password_hash, token_encrypted, last_login) VALUES (?, ?, ?, ?);";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}

	sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
	sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
	sqlite3_bind_text(stmt, 3, token_encrypted.c_str(), -1, SQLITE_TRANSIENT); // UTF-8
	sqlite3_bind_text(stmt, 4, timeLogin.c_str(), -1, SQLITE_TRANSIENT); // UTF-8

	bool success = (sqlite3_step(stmt) == SQLITE_DONE);
	sqlite3_finalize(stmt);
	return success;
}

std::string LoginDB::getToken() {
    std::string sql = "SELECT token_encrypted FROM user_account;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db.getDB(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
		return "";
	}

	std::string token = "";
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		const char* token_encrypted = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		token = token_encrypted ? token_encrypted : "";
	}

	sqlite3_finalize(stmt);
	return token;
}

AppDB::AppDB() : db(SQLiteDB::GetInstance()) {}

AppDB::~AppDB() {}

AppDB& AppDB::GetInstance() {
	static AppDB instance;
	return instance;
}

bool AppDB::add(const std::string& app_name, const std::string& version, const std::string& publisher,
    const std::string& install_location, const std::string& exe_path,
    const std::string& uninstall_string, const std::string& quiet_uninstall_string) {
    std::string currentDate = GetCurrentDate();
    std::string currentTime = GetCurrentTimeHour();
    std::string lastUpdated = currentDate + " " + currentTime;

    const char* checkSql = "SELECT id FROM installed_apps WHERE app_name = ?;";
    sqlite3_stmt* checkStmt;
    bool exists = false;
    if (sqlite3_prepare_v2(db.getDB(), checkSql, -1, &checkStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(checkStmt, 1, app_name.c_str(), -1, SQLITE_TRANSIENT);
        if (sqlite3_step(checkStmt) == SQLITE_ROW) {
            exists = true;
        }
        sqlite3_finalize(checkStmt);
    }

    if (!exists) {
        const char* insertSql = "INSERT INTO installed_apps (app_name, version, publisher, install_location, "
            "exe_path, uninstall_string, quiet_uninstall_string, status, last_updated) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, 'installed', ?);";
        sqlite3_stmt* insertStmt;
        if (sqlite3_prepare_v2(db.getDB(), insertSql, -1, &insertStmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_text(insertStmt, 1, app_name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 2, version.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 3, publisher.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 4, install_location.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 5, exe_path.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 6, uninstall_string.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 7, quiet_uninstall_string.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(insertStmt, 8, lastUpdated.c_str(), -1, SQLITE_TRANSIENT);

        bool success = (sqlite3_step(insertStmt) == SQLITE_DONE);
        sqlite3_finalize(insertStmt);
        return success;
    }
}

bool AppDB::delete_all() {
	const char* sql = "DELETE FROM installed_apps;";
	sqlite3_stmt* stmt;
	if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
		return false;
	}
	bool success = (sqlite3_step(stmt) == SQLITE_DONE);
	sqlite3_finalize(stmt);
	return success;
}

bool AppDB::update_status(const std::string& app_name, const std::string& status) {
    if (app_name.empty() || status.empty()) return false; 

    std::string currentDate = GetCurrentDate();
    std::string currentTime = GetCurrentTimeHour();
    std::string lastUpdated = currentDate + " " + currentTime;

    const char* sql = "UPDATE installed_apps SET status = ?, last_updated = ? WHERE app_name = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, lastUpdated.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, app_name.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

json AppDB::query_apps() {
    nlohmann::json jsonArray;

    const char* sql = "SELECT id, app_name, version, publisher, install_location, exe_path, "
        "uninstall_string, quiet_uninstall_string, status, last_updated "
        "FROM installed_apps WHERE install_location IS NOT NULL AND install_location != '';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return jsonArray;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        nlohmann::json jsonApp;
        jsonApp["id"] = sqlite3_column_int64(stmt, 0);
        jsonApp["app_name"] = sqlite3_column_text(stmt, 1) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) : "";
        jsonApp["version"] = sqlite3_column_text(stmt, 2) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)) : "";
        jsonApp["publisher"] = sqlite3_column_text(stmt, 3) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)) : "";
        jsonApp["install_location"] = sqlite3_column_text(stmt, 4) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)) : "";
        jsonApp["exe_path"] = sqlite3_column_text(stmt, 5) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)) : "";
        jsonApp["uninstall_string"] = sqlite3_column_text(stmt, 6) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6)) : "";
        jsonApp["quiet_uninstall_string"] = sqlite3_column_text(stmt, 7) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7)) : "";
        jsonApp["status"] = sqlite3_column_text(stmt, 8) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8)) : "";
        jsonApp["last_updated"] = sqlite3_column_text(stmt, 9) ?
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9)) : "";

        jsonArray.push_back(jsonApp);
    }

    sqlite3_finalize(stmt);
    return jsonArray;
}

ConfigMonitorDB::ConfigMonitorDB() : db(SQLiteDB::GetInstance()) {}

ConfigMonitorDB::~ConfigMonitorDB() {}

ConfigMonitorDB& ConfigMonitorDB::GetInstance() {
    static ConfigMonitorDB instance;
    return instance;
}

bool ConfigMonitorDB::add(const std::string& time_limit_daily, std::string& config_websites,
    std::string& config_apps, std::string command, std::string status) {

    // Compute the current update timestamp
    std::string currentDate = GetCurrentDate();
    std::string currentTime = GetCurrentTimeHour();
    std::string updatedAt = currentDate + " " + currentTime + ":00:00"; // Format: YYYY-MM-DD HH:00:00

    // Delete all existing records since only one config entry is needed
    const char* deleteSql = "DELETE FROM configs;";
    sqlite3_stmt* deleteStmt;
    if (sqlite3_prepare_v2(db.getDB(), deleteSql, -1, &deleteStmt, nullptr) != SQLITE_OK) {
        return false;
    }
    if (sqlite3_step(deleteStmt) != SQLITE_DONE) {
        sqlite3_finalize(deleteStmt);
        return false;
    }
    sqlite3_finalize(deleteStmt);

    // Insert the new configuration record
    const char* insertSql = "INSERT INTO configs (time_limit_daily, config_websites, config_apps, command, status, updated_at) "
        "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* insertStmt;
    if (sqlite3_prepare_v2(db.getDB(), insertSql, -1, &insertStmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(insertStmt, 1, time_limit_daily.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insertStmt, 2, config_websites.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insertStmt, 3, config_apps.c_str(), -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(insertStmt, 4, command.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insertStmt, 5, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(insertStmt, 6, updatedAt.c_str(), -1, SQLITE_TRANSIENT);

    bool success = (sqlite3_step(insertStmt) == SQLITE_DONE);
    sqlite3_finalize(insertStmt);
    return success;
}


json ConfigMonitorDB::query_config() {
    json result;
    std::string currentDate = GetCurrentDate();
    std::string currentTime = GetCurrentTimeHour();

    const char* sql = "SELECT id, time_limit_daily, config_websites, config_apps, updated_at, command, status FROM configs;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db.getDB(), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result; // Return empty JSON if query fails
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* time_limit_daily = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* config_websites = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* config_apps = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		const char* command = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const char* status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));

        // Extract date and hour from updated_at (format: YYYY-MM-DD HH:MM:SS)
        std::string updated_at_str = updated_at ? updated_at : "";
        std::string updated_date = updated_at_str.substr(0, 10); // YYYY-MM-DD
        std::string updated_hour = updated_at_str.substr(11, 2); // HH

        // Skip records matching current date and hour
        if (updated_date == currentDate && updated_hour == currentTime) {
            continue;
        }

        // Create JSON object for the record
        json record = {
            {"id", id},
            {"time_limit_daily", time_limit_daily ? time_limit_daily : ""},
            {"config_websites", config_websites ? config_websites : ""},
            {"config_apps", config_apps ? config_apps : ""},
            {"updated_at", updated_at ? updated_at : ""},
            {"status", status ? status : ""}
        };
        result.push_back(record);
    }

    sqlite3_finalize(stmt);
    return result;
}