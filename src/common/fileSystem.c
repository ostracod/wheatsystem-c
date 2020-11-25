
#include "./headers.h"

allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc) {
    heapMemoryOffset_t tempAddress = getDynamicAllocDataAddress(stringAlloc);
    heapMemoryOffset_t tempSize = getDynamicAllocSize(stringAlloc);
    return openFile(tempAddress, (uint8_t)tempSize);
}

void validateFileHandle(allocPointer_t fileHandle) {
    validateDynamicAlloc(fileHandle);
    if (unhandledErrorCode != 0) {
        return;
    }
    if (getDynamicAllocMember(fileHandle, creator) != NULL_ALLOC_POINTER) {
        unhandledErrorCode = TYPE_ERR_CODE;
        return;
    }
}


