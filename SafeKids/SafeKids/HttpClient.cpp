
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "HttpClient.h"
#include "nlohmann/json.hpp"
#include "Config.h"
#include "ComputerInfo.h"
#include "SQLiteDB.h"

#define MINE_BEGIN			"----------------------------2896059025745124%08d\r\n"\
							"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"\
							"Content-Type: application/x-msdos-program\r\n\r\n"
#define MINE_END			"\r\n----------------------------2896059025745124%08d--\r\n"
#define MINE_CONTENT_TYPE	"multipart/form-data; boundary=--------------------------2896059025745124%08d"

HttpClient::HttpClient()
{
	LoginDB& loginDB = LoginDB::GetInstance();
	Config& cfg = Config::GetInstance();
	m_sToken = loginDB.getToken();
}

HttpClient::~HttpClient() {}

HttpClient& HttpClient::GetInstance() {
	static HttpClient instance;
	return instance;
}

bool HttpClient::SendRequestGetToken(LPCSTR pszUserName, LPCSTR pszPassword)
{
	Config& cfg = Config::GetInstance();
	ComputerInfo comInfo = ComputerInfo::GetInstance();

	Client client(cfg.GetHost(), cfg.GetPort());
	//client.enable_server_certificate_verification(false);
	client.set_basic_auth(pszUserName, pszPassword);

	json jsBody;
	jsBody["deviceId"] = comInfo.GetMachineGUID();
	jsBody["deviceName"] = comInfo.GetDesktopName();
	std::cout << "Device ID: " << jsBody["deviceId"] << std::endl;
	std::cout << "Device Name: " << jsBody["deviceName"] << std::endl;
	std::string szBody = jsBody.dump();

	auto response = client.Post("/api/auth/deviceLogin", szBody, "application/json");

	if (response && response->status == 200)
	{
		auto res = json::parse(response->body);

		if (res.contains("token")) {
			m_sToken = res["token"];
			std::cout << m_sToken;
			LoginDB& loginDB = LoginDB::GetInstance();
			loginDB.add(pszUserName, pszPassword, m_sToken);
			return true;
		}
	}
	else
	{
		auto res = json::parse(response->body);
		std::cout << res["message"];
		//Log->Error("Request failed with error: %d", -1);
	}
	return false;
}

bool HttpClient::SendRequestGetPolling() {
	Config& cfg = Config::GetInstance();

	Client client(cfg.GetHost(), cfg.GetPort());

	Headers headers = {
		{ "Authorization", "Bearer " + m_sToken }
	};

	auto response = client.Get("/api/polling", headers);

	if (response && response->status == 200) {
		std::cout << "Response: " << response->body << std::endl;
	}
	else {
		std::cerr << "Request failed: " << response->status << std::endl;
		return false;
	}

	return true;
}

bool HttpClient::PushPowerUsage(json data) {
	Config& cfg = Config::GetInstance();
	Client m_client(cfg.GetHost(), cfg.GetPort());
	Headers headers = {
		{ "Authorization", "Bearer " + m_sToken }
	};
	auto response = m_client.Post("/api/kid/add-power-usage", headers, data.dump(), "application/json");
	if (response && response->status == 201) {
		std::cout << "Response: " << response->body << std::endl;
	}
	else {
		std::cerr << "Request failed: " << response->status << std::endl;
		return false;
	}
	return true;
}

bool HttpClient::PushProcessUsage(json data) {
	Config& cfg = Config::GetInstance();
	Client m_client(cfg.GetHost(), cfg.GetPort());
	Headers headers = {
		{ "Authorization", "Bearer " + m_sToken }
	};
	auto response = m_client.Post("/api/kid/add-process-usage", headers, data.dump(), "application/json");
	if (response && response->status == 201) {
		std::cout << "Response: " << response->body << std::endl;
	}
	else {
		std::cerr << "Request failed: " << response->status << std::endl;
		return false;
	}
	return true;	
}

bool HttpClient::PushApplication(json data) {
	Config& cfg = Config::GetInstance();
	Client m_client(cfg.GetHost(), cfg.GetPort());
	Headers headers = {
		{ "Authorization", "Bearer " + m_sToken }
	};
	auto response = m_client.Post("/api/kid/add-installed-apps", headers, data.dump(), "application/json");
	if (response && response->status == 201) {
		std::cout << "Response: " << response->body << std::endl;
	}
	else {
		std::cerr << "Request failed: " << response->status << std::endl;
		return false;
	}
	return true;
}

std::string HttpClient::GetToken() {
	return m_sToken;
}

json HttpClient::SendRequestGetConfig() {
	Config& cfg = Config::GetInstance();
	json result;
	Client m_client(cfg.GetHost(), cfg.GetPort());
	Headers headers = {
		{ "Authorization", "Bearer " + m_sToken }
	};
	auto response = m_client.Get("/api/kid/get-config", headers);
	if (response && response->status == 200) {
		try {
			result = json::parse(response->body)[0];
		}
		catch (const json::parse_error& e) {
			std::cerr << "JSON parse error: " << e.what() << std::endl;
			return json();
		}
	}
	else {
		std::cerr << "Request failed: " << response->status << std::endl;
		return json();
	}
	return result;
}

bool HttpClient::SendRequestUpdateOnline() {
	Config& cfg = Config::GetInstance();
	Client m_client(cfg.GetHost(), cfg.GetPort());
	Headers headers = {
		{ "Authorization", "Bearer " + m_sToken }
	};
	auto response = m_client.Post("/api/kid/update-status", headers, {""}, "application/json");
	if (response && response->status == 201) {
		std::cout << "Response: " << response->body << std::endl;
	}
	else {
		std::cerr << "Request failed: " << response->status << std::endl;
		return false;
	}

	return true;
}

bool HttpClient::SendRequestUninstall() {
	Config& cfg = Config::GetInstance();
	Client m_client(cfg.GetHost(), cfg.GetPort());
	Headers headers = {
		{ "Authorization", "Bearer " + m_sToken }
	};
	auto response = m_client.Post("/api/kid/uninstall", headers, { "" }, "application/json");
	if (response && response->status == 201) {
		std::cout << "Response: " << response->body << std::endl;
	}
	else {
		std::cerr << "Request failed: " << response->status << std::endl;
		return false;
	}

	return true;
}