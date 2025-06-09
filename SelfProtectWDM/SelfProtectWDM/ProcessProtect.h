#pragma once
#include "stdafx.h"
#include "Common.h"

typedef struct _PROTECTED_PROCESS {
    LIST_ENTRY ListEntry;
    UNICODE_STRING ProcessPath;
} PROTECTED_PROCESS, * PPROTECTED_PROCESS;

LIST_ENTRY ProtectedProcessesList;
KSPIN_LOCK ProtectedProcessesLock;

typedef struct _TD_CALLBACK_PARAMETERS {
    ACCESS_MASK AccessBitsToClear;
    ACCESS_MASK AccessBitsToSet;
}
TD_CALLBACK_PARAMETERS, * PTD_CALLBACK_PARAMETERS;

typedef struct _TD_CALLBACK_REGISTRATION {

    //
    // Handle returned by ObRegisterCallbacks.
    //

    PVOID RegistrationHandle;

    //
    // If not NULL, filter only requests to open/duplicate handles to this
    // process (or one of its threads).
    //

    PVOID TargetProcess;
    HANDLE TargetProcessId;


    //
    // Currently each TD_CALLBACK_REGISTRATION has at most one process and one
    // thread callback. That is, we can't register more than one callback for
    // the same object type with a single ObRegisterCallbacks call.
    //

    TD_CALLBACK_PARAMETERS ProcessParams;
    TD_CALLBACK_PARAMETERS ThreadParams;

    ULONG RegistrationId;        // Index in the global TdCallbacks array.

}

TD_CALLBACK_REGISTRATION, * PTD_CALLBACK_REGISTRATION;

VOID InitializeProtectedProcessesList();
NTSTATUS AddProtectedProcess(_In_ PCUNICODE_STRING ProcessPath);
VOID RemoveProtectedProcess(_In_ PCUNICODE_STRING ProcessPath);
VOID CleanupProcessProtection(VOID);    
BOOLEAN IsProtectedProcess(_In_ PCUNICODE_STRING ProcessPath);

//Init process protect
NTSTATUS InitializeProcessProtection();

// Process callback handles

PCREATE_PROCESS_NOTIFY_ROUTINE PreProcessNotifyRoutine(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create);
PCREATE_THREAD_NOTIFY_ROUTINE PreThreadNotifyRoutine(HANDLE ProcessId, HANDLE ThreadId, BOOLEAN Create);
OB_PREOP_CALLBACK_STATUS PreObjectCreateCallback(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation
);

NTSTATUS GetImagePathFromProcessId(
	HANDLE ProcessId,
	OUT PUNICODE_STRING ImagePath
);

HANDLE GetProcessIdByPeProcess(
	IN PEPROCESS Process
);

OB_PREOP_CALLBACK_STATUS PreProcessHandleOperation(
	POB_PRE_OPERATION_INFORMATION OperationInformation
);

void vigo0x1();

OB_PREOP_CALLBACK_STATUS
CBTdPreOperationCallback(
    _In_ PVOID RegistrationContext,
    _Inout_ POB_PRE_OPERATION_INFORMATION PreInfo
);

VOID UnregisterProcessProtection();