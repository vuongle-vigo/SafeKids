#pragma once
#include <string>
#include "sqlite3/sqlite3.h"
#include "nlohmann/json.hpp"
#define SQLITE_DB_PATH "C:\\Users\\levuong\\Documents\\GitHub\\SafeKids\\SafeKids\\sqlite_db\\safekids.db"

using json = nlohmann::json;

class SQLiteDB {
public:
    SQLiteDB(const std::string& dbName);
    ~SQLiteDB();
    static SQLiteDB& GetInstance();
    bool execute(const std::string& query);
    sqlite3* getDB() const { return db; }

private:
    sqlite3* db;
};

class SQLiteStmt {
public:
    SQLiteStmt(SQLiteDB& db, const std::string& query);
    ~SQLiteStmt();
    bool step();
    sqlite3_stmt* getStmt() const { return stmt; }

private:
    sqlite3_stmt* stmt;
};

class ProcessUsageDB {
public:
    ProcessUsageDB();
	~ProcessUsageDB();
    static ProcessUsageDB& GetInstance();
    bool add(const std::wstring& process_title, const std::string& process_path, const std::string& date_recorded, const std::string& start_time, double time_usage);
    bool update(int usage_id, const std::wstring& process_title, const std::string& process_path, const std::string& date_recorded, const std::string& start_time, double time_usage);
    bool update_lastest(const std::wstring& process_title, const std::string& process_path, const std::string& date_recorded, const std::string& start_time, double time_usage);
    bool remove(int usage_id);
    json query_all();
	bool delete_data(json data);
	bool update_status(json data);

private:
    SQLiteDB& db;
};

class PowerUsageDB {
public:
	PowerUsageDB();
	~PowerUsageDB();
	static PowerUsageDB& GetInstance();
	bool add(const std::string& date, int hour, double usage_minute);
	bool update(const std::string& date, int hour, double usage_minute);
	double QueryByTime(const std::string& date, int hour);
    json query_all();
    double query_today();
	bool delete_data(json data);
    bool update_status(json data);
private:
	SQLiteDB& db;
};

class LoginDB {
public:
    LoginDB();
    ~LoginDB();
    static LoginDB& GetInstance();
    bool add(const std::string& username, const std::string& password_hash, const std::string& token_encrypted);
    std::string getToken();
private:
    SQLiteDB& db;
};

class AppDB {
public:
	AppDB();
	~AppDB();
	static AppDB& GetInstance();
    bool add(const std::string& app_name, const std::string& version, const std::string& publisher, const std::string& install_location, const std::string& exe_path, const std::string& uninstall_string, const std::string& quiet_uninstall_string);
    bool update_status(const std::string& app_name, const std::string& status);
	json query_apps();
private:
	SQLiteDB& db;
};

class ConfigMonitorDB {
public:
    ConfigMonitorDB();
    ~ConfigMonitorDB();
    static ConfigMonitorDB& GetInstance();
    bool add(const std::string& time_limit_daily, std::string& config_websites, std::string& config_apps, std::string status);
    json query_config();
private:
    SQLiteDB& db;
};