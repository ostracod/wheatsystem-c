
#include "./headers.h"

allocPointer_t(fileHandle_t) openFileByStringAlloc(
    allocPointer_t(stringAlloc_t) stringAlloc
) {
    heapMemoryOffset_t tempAddress = getStringAllocDataAddress(stringAlloc);
    heapMemoryOffset_t tempSize = getStringAllocSize(stringAlloc);
    return openFile(tempAddress, (uint8_t)tempSize);
}

allocPointer_t(fileHandle_t) openFileByDynamicStringAlloc(
    allocPointer_t(dynamicAlloc_t) dynamicStringAlloc
) {
    heapMemoryOffset_t tempAddress = getDynamicAllocDataAddress(dynamicStringAlloc);
    heapMemoryOffset_t tempSize = getDynamicAllocSize(dynamicStringAlloc);
    return openFile(tempAddress, (uint8_t)tempSize);
}

void validateFileHandle(allocPointer_t(fileHandle_t) fileHandle) {
    allocPointer_t(dynamicAlloc_t) dynamicAlloc = castAllocPointer(
        fileHandle,
        dynamicAlloc_t
    );
    validateDynamicAlloc(dynamicAlloc);
    if (unhandledErrorCode != 0) {
        return;
    }
    if (!pointerIsNull(getDynamicAllocMember(dynamicAlloc, creator))) {
        unhandledErrorCode = TYPE_ERR_CODE;
        return;
    }
}


