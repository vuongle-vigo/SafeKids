#include "ProcessProtect.h"

VOID InitializeProtectedProcessesList() {
	InitializeListHead(&ProtectedProcessesList);
	KeInitializeSpinLock(&ProtectedProcessesLock);
}

NTSTATUS AddProtectedProcess(_In_ PCUNICODE_STRING ProcessPath) {
	KIRQL irql;
	PPROTECTED_PROCESS protectedProcess = ALLOC_POOL_WITH_TAG(NonPagedPool, sizeof(PROTECTED_PROCESS), STCT_TAG);
	if (!protectedProcess) {
		DEBUG("Failed to allocate memory for protected process\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlZeroMemory(protectedProcess, sizeof(PROTECTED_PROCESS));
	protectedProcess->ProcessPath.Length = ProcessPath->Length;
	protectedProcess->ProcessPath.MaximumLength = ProcessPath->MaximumLength;
	protectedProcess->ProcessPath.Buffer = ALLOC_POOL_WITH_TAG(NonPagedPool, ProcessPath->MaximumLength, STRG_TAG);
	if (!protectedProcess->ProcessPath.Buffer) {
		DEBUG("Failed to allocate memory for process path\n");
		FREE_POOL_WITH_TAG(protectedProcess, STCT_TAG);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	RtlCopyMemory(protectedProcess->ProcessPath.Buffer, ProcessPath->Buffer, ProcessPath->Length);
	protectedProcess->ProcessPath.Buffer[ProcessPath->Length / sizeof(WCHAR)] = L'\0'; // Null-terminate the string
	KeAcquireSpinLock(&ProtectedProcessesLock, &irql);
	InsertTailList(&ProtectedProcessesList, &protectedProcess->ListEntry);
	KeReleaseSpinLock(&ProtectedProcessesLock, irql);
	DEBUG("[SelfProtectWDM] Added protected process: %wZ\n", &protectedProcess->ProcessPath);
	return STATUS_SUCCESS;
}

VOID RemoveProtectedProcess(_In_ PCUNICODE_STRING ProcessPath) {
	KIRQL irql;
	KeAcquireSpinLock(&ProtectedProcessesLock, &irql);
	PLIST_ENTRY entry = ProtectedProcessesList.Flink;
	while (entry != &ProtectedProcessesList) {
		PPROTECTED_PROCESS process = CONTAINING_RECORD(entry, PROTECTED_PROCESS, ListEntry);
		if (RtlEqualUnicodeString(ProcessPath, &process->ProcessPath, TRUE)) { // TRUE = case-insensitive
			RemoveEntryList(&process->ListEntry);
			if (process->ProcessPath.Buffer) {
				FREE_POOL_WITH_TAG(process->ProcessPath.Buffer, STRG_TAG);
			}
			FREE_POOL_WITH_TAG(process, STCT_TAG);
			DEBUG("Removed protected process: %wZ\n", ProcessPath);
			break;
		}

		entry = entry->Flink;
	}
	KeReleaseSpinLock(&ProtectedProcessesLock, irql);
}

VOID CleanupProcessProtection(VOID) {
	KIRQL irql;
	PLIST_ENTRY entry;
	// Acquire the spinlock to protect the list
	KeAcquireSpinLock(&ProtectedProcessesLock, &irql);
	// Iterate through the list until it's empty
	while (!IsListEmpty(&ProtectedProcessesList)) {
		// Get the first entry
		entry = RemoveHeadList(&ProtectedProcessesList);
		PPROTECTED_PROCESS process = CONTAINING_RECORD(entry, PROTECTED_PROCESS, ListEntry);
		// Free the process path buffer if it exists
		if (process->ProcessPath.Buffer) {
			FREE_POOL_WITH_TAG(process->ProcessPath.Buffer, STRG_TAG);
			process->ProcessPath.Buffer = NULL; // Optional: Clear pointer for safety
		}
		// Free the PROTECTED_PROCESS structure
		FREE_POOL_WITH_TAG(process, STCT_TAG);
		DEBUG("Freed protected process during cleanup\n");
	}
	// Release the spinlock
	KeReleaseSpinLock(&ProtectedProcessesLock, irql);
	DEBUG("Process protection cleaned up\n");
}

BOOLEAN IsProtectedProcess(_In_ PCUNICODE_STRING ProcessPath) {
	KIRQL irql;
	KeAcquireSpinLock(&ProtectedProcessesLock, &irql);
	PLIST_ENTRY entry = ProtectedProcessesList.Flink;
	while (entry != &ProtectedProcessesList) {
		PPROTECTED_PROCESS process = CONTAINING_RECORD(entry, PROTECTED_PROCESS, ListEntry);
		if (RtlEqualUnicodeString(ProcessPath, &process->ProcessPath, TRUE)) { // TRUE = case-insensitive
			KeReleaseSpinLock(&ProtectedProcessesLock, irql);
			return TRUE;
		}
		entry = entry->Flink;
	}
	KeReleaseSpinLock(&ProtectedProcessesLock, irql);
	return FALSE;
}

NTSTATUS InitializeProcessProtection() {
	/*NTSTATUS status = PsSetCreateProcessNotifyRoutine(PreProcessNotifyRoutine, FALSE);
	if (!NT_SUCCESS(status)) {
		DEBUG("Failed to set process notify routine: 0x%X\n", status);
		return status;
	}

	return status;*/

	OB_CALLBACK_REGISTRATION  CBObRegistration = { 0 };
	OB_OPERATION_REGISTRATION CBOperationRegistrations[1] = { 0 };
	UNICODE_STRING CBAltitude = { 0 };
	TD_CALLBACK_REGISTRATION CBCallbackRegistration = { 0 };
	pCBRegistrationHandle = NULL;

	CBOperationRegistrations[0].ObjectType = PsProcessType;
	CBOperationRegistrations[0].Operations |= OB_OPERATION_HANDLE_CREATE;
	CBOperationRegistrations[0].Operations |= OB_OPERATION_HANDLE_DUPLICATE;
	CBOperationRegistrations[0].PreOperation = PreObjectCreateCallback;
	//CBOperationRegistrations[0].PostOperation = NULL;

	//CBOperationRegistrations[1].ObjectType = PsThreadType;
	//CBOperationRegistrations[1].Operations |= OB_OPERATION_HANDLE_CREATE;
	//CBOperationRegistrations[1].Operations |= OB_OPERATION_HANDLE_DUPLICATE;
	//CBOperationRegistrations[1].PreOperation = NULL;
	//CBOperationRegistrations[1].PostOperation = NULL;

	RtlInitUnicodeString(&CBAltitude, L"1000");

	CBObRegistration.Version = OB_FLT_REGISTRATION_VERSION;
	CBObRegistration.OperationRegistrationCount = 1;
	CBObRegistration.Altitude = CBAltitude;
	CBObRegistration.RegistrationContext = &CBCallbackRegistration;
	CBObRegistration.OperationRegistration = CBOperationRegistrations;

	NTSTATUS Status = ObRegisterCallbacks(
		&CBObRegistration,
		&pCBRegistrationHandle       // save the registration handle to remove callbacks later
	);

	if (!NT_SUCCESS(Status)) {
		DEBUG("Failed to register ObCallbacks: 0x%X\n", Status);
		return Status;
	}
	else {
		DEBUG("ObCallbacks registered successfully\n");
	}
}

// Pre Process callback
PCREATE_PROCESS_NOTIFY_ROUTINE PreProcessNotifyRoutine(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
{
	UNREFERENCED_PARAMETER(ParentId);
	UNREFERENCED_PARAMETER(Create);
	if (ProcessId == NULL) {
		return STATUS_INVALID_PARAMETER;
	}

	UNICODE_STRING ProcessPath;
	NTSTATUS status = GetImagePathFromProcessId(ProcessId, &ProcessPath);
	DEBUG("Process ID: %d, Image Path: %wZ\n", (ULONG)ProcessId, &ProcessPath);
	// Here you can add logic to protect the process
	KdPrint(("[SelfProtectWDM] Process %d %s\n", (ULONG)ProcessId, Create ? "created" : "deleted"));
	return STATUS_SUCCESS;
}

// Pre Thread callback
PCREATE_THREAD_NOTIFY_ROUTINE PreThreadNotifyRoutine(HANDLE ProcessId, HANDLE ThreadId, BOOLEAN Create)
{
	UNREFERENCED_PARAMETER(ProcessId);
	UNREFERENCED_PARAMETER(Create);
	if (ThreadId == NULL) {
		return STATUS_INVALID_PARAMETER;
	}
	// Here you can add logic to protect the thread
	KdPrint(("[SelfProtectWDM] Thread %d %s\n", (ULONG)ThreadId, Create ? "created" : "deleted"));
	return STATUS_SUCCESS;
}

// Pre Object Create callback
OB_PREOP_CALLBACK_STATUS PreObjectCreateCallback(
	PVOID RegistrationContext,
	POB_PRE_OPERATION_INFORMATION OperationInformation
)
{
	UNREFERENCED_PARAMETER(RegistrationContext);

	if (OperationInformation == NULL || OperationInformation->ObjectType == NULL) {
		return OB_PREOP_SUCCESS;
	}

	if (OperationInformation->ObjectType == *PsProcessType) {
		HANDLE ProcessId = GetProcessIdByPeProcess((PEPROCESS)OperationInformation->Object);
		if (ProcessId == NULL) {
			return OB_PREOP_SUCCESS;
		}

		/*UNICODE_STRING ImagePath = { 0 };
		NTSTATUS status = GetImagePathFromProcessId(ProcessId, &ImagePath);
		if (!NT_SUCCESS(status)) {
			DEBUG("Failed to get image path for process ID: %d, status: 0x%X\n", (ULONG)ProcessId, status);
			return OB_PREOP_SUCCESS;
		}
		else {
			DEBUG("Process ID: %d, Image Path: %wZ\n", (ULONG)ProcessId, &ImagePath);
			FREE_POOL_WITH_TAG(ImagePath.Buffer, STRG_TAG);
		}*/

		return PreProcessHandleOperation(OperationInformation);
	}
	else if (OperationInformation->ObjectType == *PsThreadType) {

	}

	return OB_PREOP_SUCCESS;
}


NTSTATUS GetImagePathFromProcessId(
	HANDLE ProcessId,
	OUT PUNICODE_STRING ImagePath
) {
	PUNICODE_STRING ImagePathBuffer = NULL;
	PEPROCESS Process;
	NTSTATUS status = PsLookupProcessByProcessId(ProcessId, &Process);
	if (!NT_SUCCESS(status)) {
		DEBUG("Failed to lookup process by ID: %d, status: 0x%X\n", (ULONG)ProcessId, status);
		return status;
	}

	status = SeLocateProcessImageName(Process, &ImagePathBuffer);
	if (!NT_SUCCESS(status)) {
		DEBUG("Failed to get image path for process ID: %d, status: 0x%X\n", (ULONG)ProcessId, status);
	}

	//Copy string 
	if (ImagePathBuffer == NULL || ImagePathBuffer->Length == 0) {
		goto __CLEAN;
	}

	ImagePath->Length = ImagePathBuffer->Length;
	ImagePath->MaximumLength = ImagePathBuffer->MaximumLength;
	ImagePath->Buffer = (PWCHAR)ALLOC_POOL_WITH_TAG(PagedPool, ImagePath->MaximumLength, STRG_TAG);
	RtlCopyMemory(ImagePath->Buffer, ImagePathBuffer->Buffer, ImagePath->Length);

__CLEAN:
	ObDereferenceObject(Process);
	FREE_POOL_WITH_TAG(ImagePathBuffer, STCT_TAG);
	return status;
}

HANDLE GetProcessIdByPeProcess(
	IN PEPROCESS Process
) {
	if (Process == NULL) {
		DEBUG("Invalid process object\n");
		return NULL;
	}
	HANDLE ProcessId = PsGetProcessId(Process);
	if (ProcessId == NULL) {
		DEBUG("Failed to get process ID from EPROCESS\n");
		return NULL;
	}

	return ProcessId;
}

OB_PREOP_CALLBACK_STATUS PreProcessHandleOperation(
	POB_PRE_OPERATION_INFORMATION OperationInformation
) {
	if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
		return OB_PREOP_SUCCESS;
	}

	if (!OperationInformation || !OperationInformation->Object) {
		return OB_PREOP_SUCCESS;
	}

	ACCESS_MASK DesiredAccess = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;
	if (!(
		(CHECK_FLAG(DesiredAccess, PROCESS_TERMINATE) || CHECK_FLAG(DesiredAccess, PROCESS_VM_OPERATION) ||
			CHECK_FLAG(DesiredAccess, PROCESS_VM_WRITE) || CHECK_FLAG(DesiredAccess, PROCESS_CREATE_THREAD) ||
			CHECK_FLAG(DesiredAccess, PROCESS_SUSPEND_RESUME) || CHECK_FLAG(DesiredAccess, PROCESS_SET_INFORMATION) ||
			CHECK_FLAG(DesiredAccess, WRITE_DAC) || CHECK_FLAG(DesiredAccess, WRITE_OWNER))
		)) {
		// If the access is not related to process protection, allow it
		return OB_PREOP_SUCCESS;
	}

	PEPROCESS Process = (PEPROCESS)OperationInformation->Object;
	if (Process == NULL) {
		return OB_PREOP_SUCCESS;
	}

	HANDLE ProcessId = PsGetProcessId(Process);
	if (ProcessId == NULL) {
		return OB_PREOP_SUCCESS;
	}

	UNICODE_STRING ProcessPath;
	NTSTATUS status = GetImagePathFromProcessId(ProcessId, &ProcessPath);
	if (!NT_SUCCESS(status)) {
		return OB_PREOP_SUCCESS;
	}

	if (IsProtectedProcess(&ProcessPath)) {
		DEBUG("Access to protected process: %wZ\n", &ProcessPath);
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0; // Deny access
		FREE_POOL_WITH_TAG(ProcessPath.Buffer, STRG_TAG);
		return OB_PREOP_SUCCESS;
	}

	FREE_POOL_WITH_TAG(ProcessPath.Buffer, STRG_TAG);
	return OB_PREOP_SUCCESS;
}

VOID UnregisterProcessProtection() {
	if (pCBRegistrationHandle != NULL) {
		ObUnRegisterCallbacks(pCBRegistrationHandle);
		pCBRegistrationHandle = NULL;
		DEBUG("ObCallbacks unregistered successfully\n");
	}

	CleanupProcessProtection();
}