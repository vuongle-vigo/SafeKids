#include "stdafx.h"
#include "Driver.h"
#include "FileProtect.h"
#include "Communication.h"
#include "Filter.h"
#include "ProcessProtect.h"

// DriverEntry: Initialize the driver
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;
    PDEVICE_OBJECT deviceObject = NULL;
    UNICODE_STRING deviceName, symbolicLinkName;

    InitializeProtectedFilesList();
    InitializeProtectedProcessesList();

    InitializeProcessProtection();

    // Initialize device and symbolic link names
    RtlInitUnicodeString(&deviceName, DEVICE_NAME);
    RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);

    // Create device object
    status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
    if (!NT_SUCCESS(status)) {
        KdPrint(("[SelfProtectWDM] Failed to create device: 0x%08lX\n", (ULONG)status));
        return status;
    }

    // Create symbolic link
    status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        KdPrint(("[SelfProtectWDM] Failed to create symbolic link: 0x%08lX\n", (ULONG)status));
        return status;
    }

    // Set dispatch routines
    DriverObject->MajorFunction[IRP_MJ_CREATE] = HandleFileAccess; // For compatibility
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = HandleCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = HandleDeviceControl;

    // Initialize file protection (minifilter)
    status = InitializeFilter(DriverObject);
    if (!NT_SUCCESS(status)) {
        IoDeleteSymbolicLink(&symbolicLinkName);
        IoDeleteDevice(deviceObject);
        KdPrint(("[SelfProtectWDM] Failed to initialize file protection: 0x%08lX\n", (ULONG)status));
        return status;
    }

	//printf value gFilterHandle and gServerPort
	DEBUG("[SelfProtectWDM] gFilterHandle: %p, gServerPort: %p\n", gFilterHandle, gServerPort);

    status = InitializeCommunicationPort();
    if (!NT_SUCCESS(status)) {
        FltUnregisterFilter(gFilterHandle);
        return status;
    }

    KdPrint(("[SelfProtectWDM] Driver initialized successfully\n"));
    return STATUS_SUCCESS;
}

//// DriverUnload: Cleanup routine
//VOID DriverUnload(PDRIVER_OBJECT DriverObject)
//{
//    UNICODE_STRING symbolicLinkName;
//
//    // Cleanup file protection
//    CleanupFileProtection();
//
//    RtlInitUnicodeString(&symbolicLinkName, SYMBOLIC_LINK_NAME);
//    IoDeleteSymbolicLink(&symbolicLinkName);
//    IoDeleteDevice(DriverObject->DeviceObject);
//
//    KdPrint(("[SelfProtectWDM] Driver unloaded\n"));
//}

// HandleCreateClose: Process IRP_MJ_CLOSE
NTSTATUS HandleCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    DeviceObject; // Suppress unused parameter warning

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// HandleDeviceControl: Process IOCTL requests
NTSTATUS HandleDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    ULONG controlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
    PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;

    DeviceObject; // Suppress unused parameter warning

    switch (controlCode) {
    case IOCTL_PROTECT_FILE:
        if (inputBuffer && inputBufferLength > sizeof(WCHAR)) {
            UNICODE_STRING filePath;
            RtlInitUnicodeString(&filePath, (PCWSTR)inputBuffer);
			status = AddProtectedFile(&filePath);
			if (!NT_SUCCESS(status)) {
				KdPrint(("[SelfProtectWDM] Failed to protect file: %wZ, status: 0x%08lX\n", &filePath, (ULONG)status));
			}
			else {
				KdPrint(("[SelfProtectWDM] Successfully protected file: %wZ\n", &filePath));
			}
        }
        else {
            status = STATUS_INVALID_PARAMETER;
        }

        break;
    case IOCTL_PROTECT_PROCESS:
        status = STATUS_NOT_IMPLEMENTED;
        KdPrint(("[SelfProtectWDM] IOCTL_PROTECT_PROCESS not implemented\n"));
        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        KdPrint(("[SelfProtectWDM] Invalid IOCTL: 0x%08lX\n", (ULONG)controlCode));
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}