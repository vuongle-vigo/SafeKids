#include "Communication.h"
#include "Common.h"
#include <fltUser.h>

BOOL SendMessageToDriver(MESSAGE_TYPE Type, LPCWSTR FilePath) {
    HRESULT hr;
    HANDLE hPort;

    hr = FilterConnectCommunicationPort(PORT_NAME,
        0,
        NULL,
        0,
        NULL,
        &hPort);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to connect to port, HRESULT: 0x" << std::hex << hr << std::endl;
        return FALSE;
    }

    PROTECT_MESSAGE message = { Type };
    wcsncpy_s(message.FilePath, FilePath, 259);

    DWORD bytesReturned;
    hr = FilterSendMessage(hPort,
        &message,
        sizeof(PROTECT_MESSAGE),
        NULL,
        0,
        &bytesReturned);
    if (FAILED(hr)) {
        std::wcerr << L"FilterSendMessage failed, HRESULT: 0x" << std::hex << hr << std::endl;
        CloseHandle(hPort);
        return FALSE;
    }

    CloseHandle(hPort);
    return TRUE;
}

BOOL InitSelfProtectDriver() {
	std::wstring wsCurrentDir = GetCurrentDir();
	std::wstring dbPath = wsCurrentDir + SQLITE_DB;
    std::wstring currentPath = GetCurrentProcessPath();

	if (!SendMessageToDriver(MESSAGE_ADD_PROTECTED_FILE, dbPath.c_str())) {
		std::wcerr << L"Failed to send message to driver for database path: " << dbPath << std::endl;
		return FALSE;
	}

	if (!SendMessageToDriver(MESSAGE_ADD_PROTECTED_PROCESS, currentPath.c_str())) {
		std::wcerr << L"Failed to send message to driver for current process path: " << currentPath << std::endl;
		return FALSE;
	}
}