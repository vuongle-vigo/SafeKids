#pragma once
#include <string>

class Config {
public:
	static Config& GetInstance();
	bool SetWorkDir(const std::string& szNewWorkDir);
	std::string GetHost();
	std::string GetPort();
private:
	std::string m_szWorkdir;
	std::string m_szServerHost;
	std::string m_szServerPort;

	Config();
	~Config();
	Config(const Config&) = delete;  
	Config& operator=(const Config&) = delete;
	std::string GetWorkdir();
};