#pragma once
#include <Windows.h>
#include <string>

#define PROCESS_TRAY_NAME L"SafeKidsTray.exe"
#define TRAY_PIPE_NAME L"\\\\.\\pipe\\SafeKidsTrayPipe"

#define MAX_MESSAGE_SIZE 1024

class SafeKidsTray {
public:
	SafeKidsTray();
	~SafeKidsTray();
	static SafeKidsTray& GetInstance();
	bool InitPipeServer();
	bool MessageHandle();
	bool SendMessageToTray(const std::wstring& wszMessage);
private:
	HANDLE m_hPipe;
};