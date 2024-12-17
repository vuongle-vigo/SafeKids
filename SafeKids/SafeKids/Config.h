#pragma once
#include <string>

class Config {
public:
	static Config& GetInstance();
	bool SetWorkDir(const std::string& szNewWorkDir);
	std::string GetHost();
	int GetPort();
private:
	std::string m_szWorkdir;
	std::string m_szServerHost;
	int m_serverPort;

	Config();
	~Config();
	Config(const Config&) = delete;  
	Config& operator=(const Config&) = delete;
	std::string GetWorkdir();
};