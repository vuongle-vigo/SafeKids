#pragma once
#include <string>

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