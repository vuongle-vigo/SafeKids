#pragma once

#include "stdafx.h"
#include "Common.h"

#define PORT_NAME L"\\SelfProtectWDMCommPort"
#define MAX_MESSAGE_SIZE 512

typedef enum _MESSAGE_TYPE {
    MESSAGE_ADD_PROTECTED_FILE,
    MESSAGE_REMOVE_PROTECTED_FILE,
	MESSAGE_ADD_PROTECTED_PROCESS,
	MESSAGE_REMOVE_PROTECTED_PROCESS,
} MESSAGE_TYPE;

typedef struct _PROTECT_MESSAGE {
    MESSAGE_TYPE Type;
    WCHAR FilePath[260]; // Maximum path length
} PROTECT_MESSAGE, * PPROTECT_MESSAGE;

NTSTATUS InitializeCommunicationPort();
VOID CleanupCommunicationPort(PFLT_PORT ServerPort);
NTSTATUS PortConnectNotify(
    PFLT_PORT ClientPort,
    PVOID ServerPortCookie,
    PVOID ConnectionContext,
    ULONG SizeOfContext,
    PVOID* ConnectionPortCookie
);
VOID PortDisconnectNotify(PVOID ConnectionCookie);
NTSTATUS PortMessageNotify(
    PVOID PortCookie,
    PVOID InputBuffer,
    ULONG InputBufferLength,
    PVOID OutputBuffer,
    ULONG OutputBufferLength,
    PULONG ReturnOutputBufferLength
);