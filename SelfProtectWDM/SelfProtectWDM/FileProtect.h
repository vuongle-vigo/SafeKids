#ifndef __FILE_PROTECT_H__
#define __FILE_PROTECT_H__

//#include <ntddk.h>
#include "stdafx.h"
#include "Common.h"

typedef struct _PROTECTED_FILE {
    LIST_ENTRY ListEntry;      
    UNICODE_STRING FilePath;   
} PROTECTED_FILE, * PPROTECTED_FILE;

LIST_ENTRY ProtectedFilesList;
KSPIN_LOCK ProtectedFilesLock;

VOID InitializeProtectedFilesList();
NTSTATUS AddProtectedFile(_In_ PCUNICODE_STRING FilePath);
VOID RemoveProtectedFile(_In_ PCUNICODE_STRING FilePath);
BOOLEAN IsProtectedFile(_In_ PCUNICODE_STRING FilePath);

VOID CleanupFileProtection(VOID);
NTSTATUS HandleFileAccess(PDEVICE_OBJECT DeviceObject, PIRP Irp);
FLT_PREOP_CALLBACK_STATUS PreCreateCallback(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PVOID* CompletionContext
);

FLT_POSTOP_CALLBACK_STATUS PostCreateCallback(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
);



PWCHAR GetTargetFilePath(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects
);

#endif