#include "Config.h"

Config& Config::GetInstance() {
	static Config instance;
	return instance;
}

Config::Config() {
	m_szWorkdir = GetWorkdir();
	m_szServerHost = "192.168.142.28";
	m_szServerPort = "8765";
}

Config::~Config() {

}

std::string Config::GetWorkdir() {
	std::string szWorkdir;

	return szWorkdir;
}

std::string Config::GetHost() {
	return m_szServerHost;
}

std::string Config::GetPort() {
	return m_szServerPort;
}
