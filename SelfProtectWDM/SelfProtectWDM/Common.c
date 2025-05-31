#include "Common.h"

PFLT_FILTER gFilterHandle = NULL;
PFLT_PORT gServerPort = NULL;

PWCHAR CopyUnicodeString(
    _In_ PCUNICODE_STRING Source
) {
    if (Source == NULL || Source->Buffer == NULL || Source->Length == 0 || Source->Length > Source->MaximumLength) {
		DEBUG("[SelfProtectWDM] Invalid Unicode string provided for copying.");
        return NULL;
    }

    // Check IRQL
    if (KeGetCurrentIrql() > DISPATCH_LEVEL) {
		DEBUG("[SelfProtectWDM] CopyUnicodeString called at IRQL > DISPATCH_LEVEL, returning NULL.");
        return NULL; // Don't save to alloc
    }

    if (Source->Length % sizeof(WCHAR) != 0) {
		DEBUG("[SelfProtectWDM] Source Unicode string length is not a multiple of WCHAR size.");
        return NULL;
    }

    SIZE_T allocSize = Source->Length + sizeof(WCHAR);
    PWCHAR dest = (PWCHAR)ALLOC_POOL_WITH_TAG(NonPagedPool, allocSize, 'STRG');
    if (dest == NULL) {
		DEBUG("[SelfProtectWDM] Memory allocation failed for CopyUnicodeString.");
        return NULL;
    }

    RtlCopyMemory(dest, Source->Buffer, Source->Length);
    dest[Source->Length / sizeof(WCHAR)] = L'\0'; // Null-terminate

    return dest;
}