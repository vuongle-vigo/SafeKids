#pragma once
#include "Common.h"

#define PROCESS_LOGIN_NAME L"SafeKidsLogin.exe"
#define LOGIN_PIPE_NAME L"\\\\.\\pipe\\LoginPipe"

#define MAX_MESSAGE_SIZE 1024

class Login {
public:
	Login();
	~Login();
	static Login& GetInstance();
	bool InitPipeServer();
	bool Install();
	bool Uninstall();

private:
	std::wstring m_wszLoginPath;
};