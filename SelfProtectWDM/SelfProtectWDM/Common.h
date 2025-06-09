#pragma once
#include "stdafx.h"

#define DEBUG(fmt, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "[SelfProtect] " fmt "\n", __VA_ARGS__)

#define STRG_TAG 'STRG'
#define STCT_TAG 'STCT'

extern PFLT_FILTER gFilterHandle;
extern PFLT_PORT gServerPort;
extern PVOID pCBRegistrationHandle;

#define CHECK_FLAG(flag1, flag2) (((flag1) & (flag2)) == (flag2))

#define ALLOC_POOL_WITH_TAG(PoolType, Size, Tag) \
	(PVOID)ExAllocatePoolWithTag((PoolType), (Size), (Tag))
#define FREE_POOL_WITH_TAG(Pointer, Tag) \
	do {	\
		if ((Pointer) != NULL) {	\
			ExFreePoolWithTag((Pointer), (Tag));	\
			(Pointer) = NULL;	\
		}	\
	} while (0);

#define SAFE_FREE_POOL(Pointer) \
	do {	\
		if ((Pointer) != NULL) {	\
			ExFreePool((Pointer));	\
			(Pointer) = NULL;	\
		}	\
	} while (0);

#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)

PWCHAR CopyUnicodeString(
	_In_ PCUNICODE_STRING Source
);