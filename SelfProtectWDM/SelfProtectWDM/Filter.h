#pragma once
#include "stdafx.h"
#include "Common.h"

NTSTATUS InitializeFilter(PDRIVER_OBJECT DriverObject);
NTSTATUS FilterUnloadCallback(FLT_FILTER_UNLOAD_FLAGS Flags);