#pragma once

#include "Common.h"
#include "stdafx.h"

// Device and symbolic link names
#define DEVICE_NAME L"\\Device\\SelfProtectWDM"
#define SYMBOLIC_LINK_NAME L"\\DosDevices\\SelfProtectWDM"

// IOCTL codes
#define IOCTL_BASE 0x800
#define CTL_CODE_DEVICE(type, function, method, access) \
    CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE + function, method, access)

#define IOCTL_PROTECT_FILE CTL_CODE_DEVICE(FILE_DEVICE_UNKNOWN, 0x1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_PROTECT_PROCESS CTL_CODE_DEVICE(FILE_DEVICE_UNKNOWN, 0x2, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Function prototypes
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS HandleCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS HandleDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS HandleFileAccess(PDEVICE_OBJECT DeviceObject, PIRP Irp);
