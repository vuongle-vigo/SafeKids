#include "SQLiteDB.h"
#include <iostream>

// SQLiteDB class implementation
SQLiteDB::SQLiteDB(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
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
	static SQLiteDB instance(SQLITE_DB_PATH);
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
    const wchar_t* sql = L"UPDATE process_usage SET process_title = ?, process_path = ?, date_recorded = ?, start_time = ?, time_usage = ? WHERE usage_id = (SELECT MAX(usage_id) FROM process_usage);";

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

bool ProcessUsageDB::query(int usage_id) {
    std::string query = "SELECT * FROM process_usage WHERE usage_id = " + std::to_string(usage_id) + ";";
    SQLiteStmt stmt(db, query);
    if (stmt.step()) {
        // Process the result
		int id = sqlite3_column_int(stmt.getStmt(), 0);
		const char* process_title = reinterpret_cast<const char*>(sqlite3_column_text(stmt.getStmt(), 1));
		const char* process_path = reinterpret_cast<const char*>(sqlite3_column_text(stmt.getStmt(), 2));
		const char* date_recorded = reinterpret_cast<const char*>(sqlite3_column_text(stmt.getStmt(), 3));
		const char* start_time = reinterpret_cast<const char*>(sqlite3_column_text(stmt.getStmt(), 4));
		int time_usage = sqlite3_column_int(stmt.getStmt(), 5);
		std::cout << "ID: " << id << ", Process Title: " << process_title << ", Process Path: " << process_path << ", Date Recorded: " << date_recorded << ", Start Time: " << start_time << ", Time Usage: " << time_usage << std::endl;
        return true;
    }
    return false;
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
	const char* sql = "UPDATE power_usage SET usage_minutes = ? WHERE date = ? AND hour = ?;";
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

double PowerUsageDB::query(const std::string& date, int hour) {
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
