#pragma once
#include "httplib.h"
#include <Windows.h>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace httplib;

class HttpClient {
public:
	HttpClient();
	~HttpClient();
	static HttpClient& GetInstance();
	bool SendRequestGetToken(LPCSTR pszUserName, LPCSTR pszPassword);
	bool SendRequestGetPolling();
	json SendRequestGetConfig();
	bool PushPowerUsage(json data);
	bool PushProcessUsage(json data);
	bool PushApplication(json data);
	bool SendRequestUpdateOnline();
	std::string GetToken();

private:
	std::string m_sToken;
};
