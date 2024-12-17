
//#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "json.hpp"
#include "HttpClient.h"
#include "Config.h"
#include "ComputerInfo.h"

#define MINE_BEGIN			"----------------------------2896059025745124%08d\r\n"\
							"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"\
							"Content-Type: application/x-msdos-program\r\n\r\n"
#define MINE_END			"\r\n----------------------------2896059025745124%08d--\r\n"
#define MINE_CONTENT_TYPE	"multipart/form-data; boundary=--------------------------2896059025745124%08d"
using json = nlohmann::json;
using namespace httplib;


bool HttpClient::Init()
{
	return true;
}

bool HttpClient::SendRequestGetToken(LPCSTR pszUserName, LPCSTR pszPassword)
{
	Config &cfg = Config::GetInstance();
	ComputerInfo comInfo = ComputerInfo::GetInstance();

	Client client(cfg.GetHost(), cfg.GetPort());
	//client.enable_server_certificate_verification(false);
	client.set_basic_auth(pszUserName, pszPassword);

	json jsBody;
	jsBody["deviceId"] = comInfo.GetMachineGUID();
	jsBody["deviceName"] = "Desktop";
	std::string szBody = jsBody.dump();

	auto response = client.Post("/api/auth/deviceLogin", szBody, "application/json");

	if (response && response->status == 200)
	{
		auto res = json::parse(response->body);
		
		if (res.contains("token")) {
			m_sToken = res["token"];
			std::cout << m_sToken;
		}

		if (res.contains("error") && res["error"].is_number_integer())
		{
			int error = res.at("error");
			if (!error)
			{
				if (res.contains("data") &&
					res["data"].is_object() &&
					res["data"].contains("token") &&
					res["data"]["token"].is_string()
					)
				{
					m_sToken = res["data"]["token"];
					return true;
				}
				else
				{
					//Log->Error("Token field is invalid !");
				}
			}
			else
			{
				//Log->Error("Error %d when getting api token", res["error"]);
			}
		}
		else
		{
			//Log->Error("Invalid json structure!");
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

std::string HttpClient::GetToken() {
	return m_sToken;
}

