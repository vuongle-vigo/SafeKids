#include "Config.h"

Config::Config() {
	m_szWorkdir = GetWorkdir();
	m_szServerHost = "localhost";
	m_serverPort = 3000;
}

Config::~Config() {

}

Config& Config::GetInstance() {
	static Config instance;
	return instance;
}

std::string Config::GetWorkdir() {
	return m_szWorkdir;
}

std::string Config::GetHost() {
	return m_szServerHost;
}

int Config::GetPort() {
	return m_serverPort;
}
