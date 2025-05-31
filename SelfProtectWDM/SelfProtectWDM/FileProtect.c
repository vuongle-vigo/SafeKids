#include "FileProtect.h"
#include <fltkernel.h> // Ensure direct inclusion for minifilter APIs

// Global variables
static UNICODE_STRING ProtectedFilePath = { 0 };

VOID InitializeProtectedFilesList()
{
    InitializeListHead(&ProtectedFilesList);
    KeInitializeSpinLock(&ProtectedFilesLock);
}

// Copy a Unicode string to a new allocated buffer
NTSTATUS AddProtectedFile(_In_ PCUNICODE_STRING FilePath) {
    KIRQL irql;
	PPROTECTED_FILE protectedFile = ALLOC_POOL_WITH_TAG(NonPagedPool, sizeof(PROTECTED_FILE), STCT_TAG);
	if (!protectedFile) {
		KdPrint(("[SelfProtectWDM] Failed to allocate memory for protected file\n"));
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlZeroMemory(protectedFile, sizeof(PROTECTED_FILE));
	protectedFile->FilePath.Length = FilePath->Length;
	protectedFile->FilePath.MaximumLength = FilePath->MaximumLength;
	protectedFile->FilePath.Buffer = ALLOC_POOL_WITH_TAG(NonPagedPool, FilePath->MaximumLength, STRG_TAG);
	if (!protectedFile->FilePath.Buffer) {
		KdPrint(("[SelfProtectWDM] Failed to allocate memory for file path\n"));
		FREE_POOL_WITH_TAG(protectedFile, STCT_TAG);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlCopyMemory(protectedFile->FilePath.Buffer, FilePath->Buffer, FilePath->Length);
	protectedFile->FilePath.Buffer[FilePath->Length / sizeof(WCHAR)] = L'\0'; // Null-terminate the string
	KeAcquireSpinLock(&ProtectedFilesLock, &irql);
	InsertTailList(&ProtectedFilesList, &protectedFile->ListEntry);
	KeReleaseSpinLock(&ProtectedFilesLock, irql);
	KdPrint(("[SelfProtectWDM] Added protected file: %wZ\n", &protectedFile->FilePath));
	return STATUS_SUCCESS;
}   

// Remove a protected file from the list
VOID RemoveProtectedFile(_In_ PCUNICODE_STRING FilePath) {
    KIRQL irql;
    KeAcquireSpinLock(&ProtectedFilesLock, &irql);

    PLIST_ENTRY entry = ProtectedFilesList.Flink;
    while (entry != &ProtectedFilesList) {
        PPROTECTED_FILE file = CONTAINING_RECORD(entry, PROTECTED_FILE, ListEntry);
        if (RtlEqualUnicodeString(FilePath, &file->FilePath, TRUE)) { // TRUE = case-insensitive
            RemoveEntryList(&file->ListEntry);
            if (file->FilePath.Buffer) {
                FREE_POOL_WITH_TAG(file->FilePath.Buffer, STRG_TAG);
            }

            FREE_POOL_WITH_TAG(file, STRG_TAG);
            KdPrint(("[SelfProtectWDM] Removed protected file: %wZ\n", FilePath));
            break;
        }

        entry = entry->Flink;
    }
    KeReleaseSpinLock(&ProtectedFilesLock, irql);
}

// Check if a file is protected
BOOLEAN IsProtectedFile(_In_ PCUNICODE_STRING FilePath) {
    KIRQL irql;
    KeAcquireSpinLock(&ProtectedFilesLock, &irql);
    PLIST_ENTRY entry = ProtectedFilesList.Flink;
    while (entry != &ProtectedFilesList) {
        PPROTECTED_FILE file = CONTAINING_RECORD(entry, PROTECTED_FILE, ListEntry);
        if (RtlEqualUnicodeString(FilePath, &file->FilePath, TRUE)) {
            KeReleaseSpinLock(&ProtectedFilesLock, irql);
            return TRUE;
        }

        entry = entry->Flink;
    }

    KeReleaseSpinLock(&ProtectedFilesLock, irql);
    return FALSE;
}

// Pre-operation callback
FLT_PREOP_CALLBACK_STATUS PreCreateCallback(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects,
    PVOID* CompletionContext
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);
    
    if (!Data || !FltObjects) {
        return FLT_PREOP_SUCCESS_WITH_CALLBACK;
    }

    ULONG createOptions;
    ULONG createDisposition;
    ACCESS_MASK desiredAccess;

    //
    //  24 bits low belong to createOptions
    //  8 bits high belong to createDisposition
    //
    desiredAccess = Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess;
    createOptions = Data->Iopb->Parameters.Create.Options & 0x00FFFFFF;
    createDisposition = Data->Iopb->Parameters.Create.Options >> 24;
    
    if (!(
        (CHECK_FLAG(desiredAccess, DELETE) || CHECK_FLAG(desiredAccess, WRITE_DAC) || CHECK_FLAG(desiredAccess, WRITE_OWNER)) ||
        (CHECK_FLAG(desiredAccess, GENERIC_ALL) || CHECK_FLAG(desiredAccess, GENERIC_WRITE)) ||
        (CHECK_FLAG(desiredAccess, FILE_WRITE_DATA) || CHECK_FLAG(desiredAccess, FILE_APPEND_DATA) || CHECK_FLAG(desiredAccess, FILE_WRITE_EA) || CHECK_FLAG(desiredAccess, FILE_WRITE_ATTRIBUTES)) ||
        (CHECK_FLAG(desiredAccess, MAXIMUM_ALLOWED)) ||
        (CHECK_FLAG(createOptions, FILE_DELETE_ON_CLOSE)) ||
        (!CHECK_FLAG(createDisposition, FILE_OPEN_IF) && !CHECK_FLAG(createDisposition, FILE_OPEN))
        )) {
		return FLT_PREOP_SUCCESS_WITH_CALLBACK;
    }

    PWCHAR filePath = NULL;
	filePath = GetTargetFilePath(Data, FltObjects);
    if (filePath) {
		DEBUG("[SelfProtectWDM] PreCreateCallback: Target file path: %ws\n", filePath);
    }

    UNICODE_STRING FilePath;
	RtlInitUnicodeString(&FilePath, filePath);
	if (IsProtectedFile(&FilePath)) {
		DEBUG("Access to protected file: %wZ\n", &FilePath);
		Data->IoStatus.Status = STATUS_ACCESS_DENIED;
		Data->IoStatus.Information = 0;
		if (filePath) {
			FREE_POOL_WITH_TAG(filePath, STRG_TAG);
		}

		return FLT_PREOP_COMPLETE;
	}

    return FLT_PREOP_SUCCESS_WITH_CALLBACK;
}

// Post-operation callback 
FLT_POSTOP_CALLBACK_STATUS PostCreateCallback(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ PVOID CompletionContext,
    _In_ FLT_POST_OPERATION_FLAGS Flags
) {
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);
	UNREFERENCED_PARAMETER(Data);
	if (Data->IoStatus.Status == STATUS_ACCESS_DENIED) {
		KdPrint(("[SelfProtectWDM] Access denied for file operation\n"));
	}

	return FLT_POSTOP_FINISHED_PROCESSING;
}

// Cleanup file protection
VOID CleanupFileProtection(VOID)
{
    if (ProtectedFilePath.Buffer) {
        ExFreePool(ProtectedFilePath.Buffer);
        ProtectedFilePath.Buffer = NULL;
        ProtectedFilePath.Length = 0;
        ProtectedFilePath.MaximumLength = 0;
    }
    KdPrint(("[SelfProtectWDM] File protection cleaned up\n"));
}

// HandleFileAccess (not used in minifilter)
NTSTATUS HandleFileAccess(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_NOT_IMPLEMENTED;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_NOT_IMPLEMENTED;
}

PWCHAR GetTargetFilePath(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects
)
{
    UNREFERENCED_PARAMETER(Data);
    UNREFERENCED_PARAMETER(FltObjects);

    PFLT_FILE_NAME_INFORMATION nameInfo = NULL;
	PWCHAR filePath = NULL;
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    if (FltObjects->FileObject != NULL) {
        ntStatus = FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo);
        if (!NT_SUCCESS(ntStatus)) {
			//KdPrint(("[SelfProtectWDM] Failed to get file name information, try with FLT_FILE_NAME_OPENED: 0x%08lX\n", (ULONG)ntStatus));
			ntStatus = FltGetFileNameInformation(Data, FLT_FILE_NAME_OPENED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo);
        }
    }

	if (!NT_SUCCESS(ntStatus)) {
		//KdPrint(("[SelfProtectWDM] Failed to get file name information: 0x%08lX\n", (ULONG)ntStatus));
		return NULL;
	}

    if (nameInfo->Name.Buffer != NULL && nameInfo->Name.Length > 0) {
        filePath = CopyUnicodeString(&nameInfo->Name);
		if (filePath == NULL) {
			//KdPrint(("[SelfProtectWDM] Memory allocation failed for file path\n"));
            FltReleaseFileNameInformation(nameInfo);
			return NULL;
		}

		FltReleaseFileNameInformation(nameInfo);
    }

    if (filePath == NULL) {
		//KdPrint(("[SelfProtectWDM] File path is NULL after processing\n"));
	}
    else {
        //KdPrint(("[SelfProtectWDM] Target file path: %ws\n", filePath));
    }

    return filePath;
}