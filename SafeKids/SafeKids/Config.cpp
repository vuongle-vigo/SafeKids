#include "Config.h"
#include "Common.h"

Config::Config() {
	m_wszWorkdir = GetCurrentDir();
	m_szServerHost = "localhost";
	m_serverPort = 3000;
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
