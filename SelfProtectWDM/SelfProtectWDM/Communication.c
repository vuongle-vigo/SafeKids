#include "Communication.h"
#include "Common.h"
#include "FileProtect.h"
#include "ProcessProtect.h"

PFLT_PORT gClientPort = NULL; // Global client port handle

NTSTATUS InitializeCommunicationPort()
{
    NTSTATUS status;
    UNICODE_STRING portName;
    OBJECT_ATTRIBUTES oa;
    PSECURITY_DESCRIPTOR sd;

    if (!gFilterHandle) {
        KdPrint(("[SelfProtectWDM] Invalid FilterHandle\n"));
        return STATUS_INVALID_PARAMETER;
    }

    status = FltBuildDefaultSecurityDescriptor(&sd, FLT_PORT_ALL_ACCESS);
    if (!NT_SUCCESS(status)) {
        KdPrint(("[SelfProtectWDM] Failed to build security descriptor: 0x%08lX\n", status));
        return status;
    }

    RtlInitUnicodeString(&portName, PORT_NAME);
    InitializeObjectAttributes(&oa, &portName, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, sd);
    status = RtlSetDaclSecurityDescriptor(sd, TRUE, NULL, FALSE);

    if (gFilterHandle) {
        status = FltCreateCommunicationPort(gFilterHandle,
            &gServerPort,
            &oa,
            NULL,
            PortConnectNotify,
            PortDisconnectNotify,
            PortMessageNotify,
            1); // Max connections
    }

    FltFreeSecurityDescriptor(sd);

    if (!NT_SUCCESS(status)) {
        KdPrint(("[SelfProtectWDM] Failed to create communication port: 0x%08lX\n", status));
    }
    else {
        KdPrint(("[SelfProtectWDM] Communication port created: %wZ\n", &portName));
    }

    return status;
}

VOID CleanupCommunicationPort(PFLT_PORT ServerPort)
{
    if (ServerPort) {
        FltCloseCommunicationPort(ServerPort);
        KdPrint(("[SelfProtectWDM] Communication port closed\n"));
    }
    if (gClientPort) {
        FltCloseClientPort(gFilterHandle, &gClientPort);
        gClientPort = NULL;
    }
}

NTSTATUS PortConnectNotify(
    PFLT_PORT ClientPort,
    PVOID ServerPortCookie,
    PVOID ConnectionContext,
    ULONG SizeOfContext,
    PVOID* ConnectionPortCookie
)
{
    UNREFERENCED_PARAMETER(ServerPortCookie);
    UNREFERENCED_PARAMETER(ConnectionContext);
    UNREFERENCED_PARAMETER(SizeOfContext);
    UNREFERENCED_PARAMETER(ConnectionPortCookie);

    gClientPort = ClientPort;
    KdPrint(("[SelfProtectWDM] Client connected to port\n"));
    return STATUS_SUCCESS;
}

VOID PortDisconnectNotify(PVOID ConnectionCookie)
{
    UNREFERENCED_PARAMETER(ConnectionCookie);

    if (gClientPort) {
        FltCloseClientPort(gFilterHandle, &gClientPort);
        gClientPort = NULL;
        KdPrint(("[SelfProtectWDM] Client disconnected from port\n"));
    }
}

NTSTATUS PortMessageNotify(
    PVOID PortCookie,
    PVOID InputBuffer,
    ULONG InputBufferLength,
    PVOID OutputBuffer,
    ULONG OutputBufferLength,
    PULONG ReturnOutputBufferLength
)
{
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING FilePath;
    UNREFERENCED_PARAMETER(PortCookie);
    UNREFERENCED_PARAMETER(OutputBuffer);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(ReturnOutputBufferLength);

    if (InputBuffer && InputBufferLength == sizeof(PROTECT_MESSAGE)) {
        PPROTECT_MESSAGE message = (PPROTECT_MESSAGE)InputBuffer;
        if (message->FilePath[0] != L'\0') {
            switch (message->Type) {
            case MESSAGE_ADD_PROTECTED_FILE:
				RtlInitUnicodeString(&FilePath, message->FilePath);
                status = AddProtectedFile(&FilePath);
                KdPrint(("[SelfProtectWDM] %s adding file: %ws\n", NT_SUCCESS(status) ? "Succeeded" : "Failed", message->FilePath));
                break;
            case MESSAGE_REMOVE_PROTECTED_FILE:
                RtlInitUnicodeString(&FilePath, message->FilePath);
                RemoveProtectedFile(&FilePath);
                KdPrint(("[SelfProtectWDM] Removed file: %ws\n", message->FilePath));
                break;
            case MESSAGE_ADD_PROTECTED_PROCESS:
				RtlInitUnicodeString(&FilePath, message->FilePath);
				status = AddProtectedProcess(&FilePath);
				KdPrint(("[SelfProtectWDM] %s adding process: %ws\n", NT_SUCCESS(status) ? "Succeeded" : "Failed", message->FilePath));
				break;
            case MESSAGE_REMOVE_PROTECTED_PROCESS:
				RtlInitUnicodeString(&FilePath, message->FilePath);
				RemoveProtectedProcess(&FilePath);
				KdPrint(("[SelfProtectWDM] Removed process: %ws\n", message->FilePath));
				break;
            default:
                status = STATUS_INVALID_PARAMETER;
                KdPrint(("[SelfProtectWDM] Invalid message type\n"));
            }
        }
        else {
            status = STATUS_INVALID_PARAMETER;
            KdPrint(("[SelfProtectWDM] Empty file path\n"));
        }
    }
    else {
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("[SelfProtectWDM] Invalid input buffer or length\n"));
    }

    return status;
}
