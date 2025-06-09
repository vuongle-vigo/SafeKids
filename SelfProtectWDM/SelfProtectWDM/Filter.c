#include "Filter.h"
#include "FileProtect.h"
#include "ProcessProtect.h"

// Filter registration
const FLT_OPERATION_REGISTRATION Callbacks[] = {
    { IRP_MJ_CREATE, 0, PreCreateCallback, PostCreateCallback },
    { IRP_MJ_OPERATION_END }
};

const FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),
    FLT_REGISTRATION_VERSION,
    0,
    NULL,
    Callbacks,
    FilterUnloadCallback,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

// Initialize file protection
NTSTATUS InitializeFilter(PDRIVER_OBJECT DriverObject)
{
    NTSTATUS status;

    // Register filter
    status = FltRegisterFilter(DriverObject, &FilterRegistration, &gFilterHandle);
    if (!NT_SUCCESS(status)) {
        KdPrint(("[SelfProtectWDM] Failed to register filter: 0x%08lX\n", (ULONG)status));
        return status;
    }

    // Start filtering
    status = FltStartFiltering(gFilterHandle);
    if (!NT_SUCCESS(status)) {
        FltUnregisterFilter(gFilterHandle);
        gFilterHandle = NULL;
        KdPrint(("[SelfProtectWDM] Failed to start filtering: 0x%08lX\n", (ULONG)status));
        return status;
    }

    KdPrint(("[SelfProtectWDM] File protection initialized\n"));
    return STATUS_SUCCESS;
}


// Filter unload callback
NTSTATUS FilterUnloadCallback(FLT_FILTER_UNLOAD_FLAGS Flags)
{
    UNREFERENCED_PARAMETER(Flags);
    DEBUG("UNLOAD DRIVER");
    if (gFilterHandle) {
        FltUnregisterFilter(gFilterHandle);
        DEBUG("Unregistered filter");
        gFilterHandle = NULL;
    }

    CleanupFileProtection();
    KdPrint(("[SelfProtectWDM] Filter unloaded\n"));
    return STATUS_SUCCESS;
}