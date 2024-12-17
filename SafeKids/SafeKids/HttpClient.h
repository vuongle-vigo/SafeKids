#pragma once
#include <Windows.h>
#include <string>

namespace httpclient
{
	int HttpUploadFile(
		_In_ std::string hostAddr,
		_In_ int port,
		_In_ std::wstring httpVerd,
		_In_ std::wstring filePath,
		_Out_ int& httpStatusCode,
		_Out_ std::string& link
	);

	int HttpDownloadFile(
		_In_ std::string hostAddr,
		_In_ int port,
		_In_ std::wstring httpVerd,
		_In_ std::wstring destination,
		_Out_ int& httpStatusCode
	);
};

class HttpClient  {
public:
	bool Init();
	bool SendRequestGetToken(LPCSTR pszUserName, LPCSTR pszPassword);
	bool SendRequestGetPolling();
	std::string GetToken();
	int SendTaskResult(std::string& result);

private:
	std::string m_sUrl;
	int m_port;
	std::string m_sToken;
};
