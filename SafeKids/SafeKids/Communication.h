#pragma once
#include <windows.h>
#include <fltUser.h>

#define PORT_NAME L"\\SelfProtectWDMCommPort"
#define MAX_MESSAGE_SIZE 512

typedef enum _MESSAGE_TYPE {
	MESSAGE_ADD_PROTECTED_FILE,
	MESSAGE_REMOVE_PROTECTED_FILE,
	MESSAGE_ADD_PROTECTED_PROCESS,
	MESSAGE_REMOVE_PROTECTED_PROCESS,
} MESSAGE_TYPE;


// Message structure (must match driver)
struct PROTECT_MESSAGE {
	MESSAGE_TYPE Type;
	WCHAR FilePath[260];
};

BOOL SendMessageToDriver(MESSAGE_TYPE Type, LPCWSTR FilePath);
